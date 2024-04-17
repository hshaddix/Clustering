#include <ap_int.h>
#include <hls_stream.h>
#include "processHits.h"

void processHits(hls::stream<InputData> &inputBinariesStream, hls::stream<OutputData> &outputClustersStream) {
    #pragma HLS INTERFACE s_axilite port=return
    #pragma HLS INTERFACE axis port=inputBinariesStream
    #pragma HLS INTERFACE axis port=outputClustersStream

    Hit first_hit, second_hit;
    bool last = false, init = true;

    if (!inputBinariesStream.empty()) {
        first_hit = {0, 0}; // Initialize first_hit

        do {
            #pragma HLS PIPELINE
            InputData inputData = inputBinariesStream.read();
            last = inputData.last;

            ap_uint<16> inputBinary = inputData.data;
            second_hit.ABCStarID = inputBinary.range(15, 11);
            second_hit.position = inputBinary.range(10, 3);
            ap_uint<3> sizeBitmask = inputBinary.range(2, 0);

            // Adjust second_hit position based on bitmask
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
                    // Do nothing for the unexpected bitmask
                    break;
            }

            // Check if first and second hit are adjacent
            bool isAdjacent = (second_hit.ABCStarID == first_hit.ABCStarID && second_hit.position == first_hit.position + 1) ||
                              (second_hit.ABCStarID == first_hit.ABCStarID + 1 && first_hit.position == ABCStar_SIZE - 1 && second_hit.position == 0);

            if (!isAdjacent && !init) {
                // Output the first_hit
                OutputData outputData;
                outputData.data = (first_hit.position << 4) | 1; // Example packing, assuming size is 1 for simplicity
                outputData.last = 0;
                outputClustersStream.write(outputData);
                first_hit = second_hit; // Move second hit to first for next iteration
            }

            init = false;
        } while (!last);

        // Output the last cluster
        OutputData outputData;
        outputData.data = (first_hit.position << 4) | 1; // Example packing, assuming size is 1 for simplicity
        outputData.last = 1; // This is the last output
        outputClustersStream.write(outputData);
    }
}
