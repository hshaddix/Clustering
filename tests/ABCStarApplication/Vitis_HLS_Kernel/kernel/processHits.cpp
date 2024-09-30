#include "processHits.h"
#include <iostream>

// Define a new struct for input data to encapsulate it in a stream
// typedef ap_axiu<16, 0, 0, 1> InputData;  // 16-bit data

// Define a new struct for output data to encapsulate it in a stream
// typedef ap_axiu<(POSITION_BITS + SIZE_BITS), 0, 0, 1> OutputData; // Data width sum of parts

// Checks if two hits are adjacent considering the size of the cluster
static bool areAdjacent(const Hit &first_hit, const Hit &second_hit, bool &error) {
    if (first_hit.position + first_hit.size > second_hit.position) {
        error = true;
        return false;
    }
    error = false;
    return (first_hit.position + first_hit.size == second_hit.position);
}

// Outputs a cluster to the stream
static void outputCluster(const Hit &hit, hls::stream<OutputData> &stream, bool isLast = false) {
    // OutputData outputData;
    // outputData.data = (hit.position << SIZE_BITS) | hit.size;
    // outputData.last = isLast ? 1 : 0;
    stream << hit;
//    std::cout << "Output cluster: Position = " << hit.position << ", Size = " << hit.size << ", Last = " << outputData.last << std::endl;
}

// Outputs an error message to the stream
static void outputError(hls::stream<OutputData> &stream) {
    Hit outputData;
    outputData.position = 0xFFF;
    outputData.size = 0xFFF;
    stream << outputData;
  //  std::cout << "Output error: FFF" << std::endl;
}

// Function to process hits and cluster them
static void processHitsStream(hls::stream<InputData> &inputBinariesStream, hls::stream<OutputData> &outputClustersStream) {
    Hit first_hit, second_hit, third_hit;
    bool init = true, last = false;

    while (!inputBinariesStream.empty()) {
        InputData inputData = inputBinariesStream.read();

        // For streaming
        // last = inputData.last;
        // int ABCStarID = inputData.data.range(15, 11);
        // int basePosition = inputData.data.range(10, 3);
        // ap_uint<3> sizeBitmask = inputData.data.range(2, 0);

        // For memory mapped
        last = inputBinariesStream.empty();
        int ABCStarID = inputData.range(15, 11);
        int basePosition = inputData.range(10, 3);
        ap_uint<3> sizeBitmask = inputData.range(2, 0);
        second_hit.position = (ABCStarID << 8) | basePosition;
        third_hit.size = 0; // Initialize third_hit size to zero

    //    std::cout << "Read Data: Position = " << second_hit.position << ", Bitmask = " << sizeBitmask << std::endl;

        // Calculate the size of the current hit based on the bitmask
        switch (sizeBitmask.to_uint()) {
            case 0: // 000
                second_hit.size = 1;
                break;
            case 1: // 001
                second_hit.size = 1;
                third_hit.position = (ABCStarID << 8) | (basePosition + 3);
                third_hit.size = 1;
                break;
            case 2: // 010
                second_hit.size = 1;
                third_hit.position = (ABCStarID << 8) | (basePosition + 2);
                third_hit.size = 1;
                break;
            case 3: // 011
                second_hit.size = 1;
                third_hit.position = (ABCStarID << 8) | (basePosition + 2);
                third_hit.size = 2;
                break;
            case 4: // 100
                second_hit.size = 2;
                break;
            case 5: // 101
                second_hit.size = 2;
                third_hit.position = (ABCStarID << 8) | (basePosition + 3);
                third_hit.size = 1;
                break;
            case 6: // 110
                second_hit.size = 3;
                break;
            case 7: // 111
                second_hit.size = 4;
                break;
            default:
                break;
        }

        bool error = false;
        if (third_hit.size == 0) {
            if (!init) {
                if (!areAdjacent(first_hit, second_hit, error)) {
                    if (error) {
                        outputError(outputClustersStream);
                        init = true;
                        continue;
                    }
                    outputCluster(first_hit, outputClustersStream);
                    first_hit = second_hit;
                } else {
                    first_hit.size += second_hit.size;
                }
            } else {
                first_hit = second_hit;
                init = false;
            }
        } else {
            if (!init) {
                if (!areAdjacent(first_hit, second_hit, error)) {
                    if (error) {
                        outputError(outputClustersStream);
                        init = true;
                        continue;
                    }
                    outputCluster(first_hit, outputClustersStream);
                    outputCluster(second_hit, outputClustersStream);
                } else {
                    first_hit.size += second_hit.size;
                    outputCluster(first_hit, outputClustersStream);
                }
                first_hit = third_hit;
            } else {
                outputCluster(second_hit, outputClustersStream);
                first_hit = third_hit;
                init = false;
            }
        }

        if (last) {
            outputCluster(first_hit, outputClustersStream, true);
            init = true;
        }
    }

    if (!init) {
        outputCluster(first_hit, outputClustersStream, true);
    }
}


//////////////////////////////////////////////
//// Stream pipelined
//////////////////////////////////////////////
// Read Data from Global Memory and write into Stream inStream
static void read_input(InputData* in, hls::stream<InputData>& inStream) {
// Auto-pipeline is going to apply pipeline to this loop
READ_DATA:
    constexpr int totalSize = INPUTDATASIZE;
    for (int i = 0; i < totalSize; i++) 
    {
        #pragma HLS LOOP_TRIPCOUNT min = totalSize max = totalSize
        #pragma HLS unroll
        // Blocking write command to inStream
        inStream << in[i];
    }
}

// Read result from outStream_test and write the result to Global Memory
static void write_result(OutputData* out, hls::stream<OutputData>& outStream_test) {
// Auto-pipeline is going to apply pipeline to this loop
WRITE_DATA:
    int i = 0;
    while (!outStream_test.empty()) {
        out[i] = outStream_test.read();
        i++;
    }
}



//////////////////////////////////////////////
//// Vector pipelined
//////////////////////////////////////////////
void processHits(InputData* in1_gmem, OutputData* out1_gmem)
{
    #pragma HLS INTERFACE m_axi port=in1_gmem  offset=slave bundle=gmem 
    #pragma HLS INTERFACE m_axi port=out1_gmem offset=slave bundle=gmem 
    // #pragma HLS INTERFACE s_axilite port=in1_gmem   bundle=control
    // #pragma HLS INTERFACE s_axilite port=out1_gmem  bundle=control 
    // #pragma HLS INTERFACE s_axilite port=return     bundle=control 


    static hls::stream<InputData> inStream("input_stream");
    static hls::stream<OutputData> outStream_test("output_stream");
    #pragma HLS STREAM variable = inStream depth = 20
    #pragma HLS STREAM variable = outStream_test depth = 20
    // //  HLS STREAM variable=<name> depth=<size> pragma is used to define the Stream
    // //  depth. For this example, Depth 32 is defined. Which means that Stream can
    // //  hold
    // //  maximum 32 outstanding elements at a given time. If Stream is full, any
    // //  further
    // //  blocking write command from producer will go into wait state until consumer
    // //  reads some elements from stream. Similarly if Stream is empty (no element in
    // //  Stream)
    // //  any blocking read command from consumer will go into wait state until
    // //  producer
    // //  writes elements to Stream. This blocking read and write allow consumer and
    // //  producer to synchronize each other.

    #pragma HLS dataflow
        read_input(in1_gmem, inStream);
        processHitsStream(inStream, outStream_test);
        write_result(out1_gmem, outStream_test);



}

