#include <ap_int.h>
#include <hls_stream.h>
#include <ap_axi_sdata.h>

// Define a new struct for input data to encapsulate it in a stream
typedef ap_axiu<16, 0, 0, 1> InputData;  // 16-bit data

// Define a new struct for output data to encapsulate it in a stream
typedef ap_axiu<(POSITION_BITS + SIZE_BITS), 0, 0, 1> OutputData; // Data width sum of parts

// Checks if two hits are adjacent considering the size of the cluster
bool areAdjacent(const Hit &first_hit, const Hit &second_hit) {
    return (first_hit.position + first_hit.size == second_hit.position);
}

// Outputs a cluster to the stream
void outputCluster(const Hit &hit, hls::stream<OutputData> &stream, bool isLast = false) {
    OutputData outputData;
    outputData.data = (hit.position << SIZE_BITS) | hit.size;
    outputData.last = isLast ? 1 : 0;
    stream.write(outputData);
    std::cout << "Output cluster: Position = " << hit.position << ", Size = " << hit.size << ", Last = " << outputData.last << std::endl;
}

// Function to process hits and cluster them
void processHits(hls::stream<InputData> &inputBinariesStream, hls::stream<OutputData> &outputClustersStream) {
    #pragma HLS INTERFACE s_axilite port=return
    #pragma HLS INTERFACE axis port=inputBinariesStream
    #pragma HLS INTERFACE axis port=outputClustersStream

    Hit first_hit, second_hit, third_hit;
    bool init = true, last = false;

    while (!inputBinariesStream.empty()) {
        InputData inputData = inputBinariesStream.read();
        last = inputData.last;

        int ABCStarID = inputData.data.range(15, 11);
        int basePosition = inputData.data.range(10, 3);
        ap_uint<3> sizeBitmask = inputData.data.range(2, 0);
        second_hit.position = (ABCStarID << 8) | basePosition;

        std::cout << "Read Data: Position = " << second_hit.position << ", Bitmask = " << sizeBitmask << std::endl;

        switch (sizeBitmask.to_uint()) {
            case 0: // 000
                second_hit.size = 1;
                outputCluster(second_hit, outputClustersStream, last);
                break;

            case 1: // 001
                second_hit.size = 1;
                outputCluster(second_hit, outputClustersStream);
                third_hit.position = (ABCStarID << 8) | (basePosition + 3);
                third_hit.size = 1;
                outputCluster(third_hit, outputClustersStream, last);
                break;

            case 2: // 010
                second_hit.size = 1;
                outputCluster(second_hit, outputClustersStream);
                third_hit.position = (ABCStarID << 8) | (basePosition + 2);
                third_hit.size = 1;
                outputCluster(third_hit, outputClustersStream, last);
                break;

            case 3: // 011
                second_hit.size = 1;
                outputCluster(second_hit, outputClustersStream);
                second_hit.position = (ABCStarID << 8) | (basePosition + 2);
                second_hit.size = 2;
                outputCluster(second_hit, outputClustersStream, last);
                break;

            case 4: // 100
                second_hit.size = 1;
                outputCluster(second_hit, outputClustersStream);
                break;

            case 5: // 101
                second_hit.size = 1;
                outputCluster(second_hit, outputClustersStream);
                second_hit.position = (ABCStarID << 8) | (basePosition + 1);
                second_hit.size = 1;
                outputCluster(second_hit, outputClustersStream);
                third_hit.position = (ABCStarID << 8) | (basePosition + 3);
                third_hit.size = 1;
                outputCluster(third_hit, outputClustersStream, last);
                break;

            case 6: // 110
                second_hit.size = 2;
                outputCluster(second_hit, outputClustersStream, last);
                break;

            case 7: // 111
                second_hit.size = 3;
                outputCluster(second_hit, outputClustersStream, last);
                break;

            default:
                break;
        }

        if (!init && !areAdjacent(first_hit, second_hit)) {
            outputCluster(first_hit, outputClustersStream);
        } else {
            second_hit.size += first_hit.size;
        }

        first_hit = second_hit;
        init = false;
    }

    if (!init) {
        outputCluster(first_hit, outputClustersStream, true);
    }
}
