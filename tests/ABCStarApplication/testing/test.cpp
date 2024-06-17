#include <ap_int.h>
#include <hls_stream.h>
#include <ap_axi_sdata.h>
#include "processHits.h"

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
        third_hit.size = 0; // Initialize third_hit size to zero

        std::cout << "Read Data: Position = " << second_hit.position << ", Bitmask = " << sizeBitmask << std::endl;

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

        if (third_hit.size == 0) {
            if (!init) {
                if (!areAdjacent(first_hit, second_hit)) {
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
                if (!areAdjacent(first_hit, second_hit)) {
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
    }

    if (!init) {
        outputCluster(first_hit, outputClustersStream, true);
    }
}
