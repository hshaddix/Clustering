#include "processHits.h"

// Function to process hits and cluster them
void processHits(hls::stream<InputData> &inputBinariesStream, hls::stream<OutputData> &outputClustersStream) {
    #pragma HLS INTERFACE s_axilite port=return
    #pragma HLS INTERFACE axis port=inputBinariesStream
    #pragma HLS INTERFACE axis port=outputClustersStream

    Hit first_hit, second_hit;
    bool init = true, last = false;
    int clusterSize = 0;

    if (!inputBinariesStream.empty()) {
        do {
            #pragma HLS PIPELINE
            InputData inputData = inputBinariesStream.read();
            last = inputData.last;

            if (!init) {
                first_hit = second_hit;  // Prepare the first_hit for comparison
            }

            // Decode the input data
            second_hit.ABCStarID = inputData.data.range(15, 11);
            second_hit.position = inputData.data.range(10, 3);
            ap_uint<3> sizeBitmask = inputData.data.range(2, 0);

            // Decode bitmask and adjust the position of second_hit accordingly
            switch(sizeBitmask.to_uint()) {
                case 1: // 001
                    second_hit.position += 3;
                    break;
                case 2: // 010
                    second_hit.position += 2;
                    break;
                case 3: // 011
                    second_hit.position += 2;
                    second_hit.position += 3;
                    break;
                case 4: // 100
                    second_hit.position += 1;
                    break;
                case 5: // 101
                    second_hit.position += 1;
                    second_hit.position += 3;
                    break;
                case 6: // 110
                    second_hit.position += 1;
                    second_hit.position += 2;
                    break;
                case 7: // 111
                    second_hit.position += 1;
                    second_hit.position += 2;
                    second_hit.position += 3;
                    break;
                default:
                    // No action on default
                    break;
            }

            // Check if the first and second hits are adjacent
            bool isAdjacent = (first_hit.ABCStarID == second_hit.ABCStarID && second_hit.position == first_hit.position + 1) ||
                              (first_hit.ABCStarID + 1 == second_hit.ABCStarID && first_hit.position == ABCStar_SIZE - 1 && second_hit.position == 0);

            if (!isAdjacent && !init) {
                // Output the cluster information
                OutputData outputData;
                outputData.data = (first_hit.ABCStarID << (POSITION_BITS + SIZE_BITS)) | (first_hit.position << SIZE_BITS) | clusterSize; // Pack ABCStarID, position, and size
                outputData.last = 0;
                outputClustersStream.write(outputData);

                // Reset for the next cluster
                clusterSize = 1;
            } else {
                // Increment the cluster size if adjacent or initialize if it's the start
                clusterSize++;
            }

            init = false;
        } while (!last);

        // Output the last cluster
        OutputData outputData;
        outputData.data = (second_hit.ABCStarID << (POSITION_BITS + SIZE_BITS)) | (second_hit.position << SIZE_BITS) | clusterSize; // Pack ABCStarID, position, and size
        outputData.last = 1; // Mark this as the last cluster output
        outputClustersStream.write(outputData);
    }
}
