#include "processHits.h"

// Function to process hits and cluster them
void processHits(hls::stream<InputData> &inputBinariesStream, hls::stream<OutputData> &outputClustersStream) {
    #pragma HLS INTERFACE s_axilite port=return
    #pragma HLS INTERFACE axis port=inputBinariesStream
    #pragma HLS INTERFACE axis port=outputClustersStream

    Hit first_hit, second_hit;
    bool init = true, last = false;
    int clusterSize = 0;

    while (!inputBinariesStream.empty()) {  // Ensure there is data before starting to read
        InputData inputData = inputBinariesStream.read();
        last = inputData.last;

        std::cout << "Read Data: ABCStarID = " << inputData.data.range(15, 11) << ", Position = " << inputData.data.range(10, 3) << ", Bitmask = " << inputData.data.range(2, 0) << std::endl;

        if (!init) {
            first_hit = second_hit;  // Prepare the first_hit for comparison
            std::cout << "Set first_hit: ABCStarID = " << first_hit.ABCStarID << ", Position = " << first_hit.position << std::endl;
        }

        // Decode the input data
        second_hit.ABCStarID = inputData.data.range(15, 11);
        second_hit.position = inputData.data.range(10, 3);
        ap_uint<3> sizeBitmask = inputData.data.range(2, 0);

        std::cout << "Initial second_hit: ABCStarID = " << second_hit.ABCStarID << ", Position = " << second_hit.position << std::endl;

        // Decode bitmask and adjust the position of second_hit accordingly
        switch(sizeBitmask.to_uint()) {
            case 1: // 001
                second_hit.position += 3;
                std::cout << "Adjusted by +3: Position = " << second_hit.position << std::endl;
                break;
            case 2: // 010
                second_hit.position += 2;
                std::cout << "Adjusted by +2: Position = " << second_hit.position << std::endl;
                break;
            case 3: // 011
                second_hit.position += 2;
                std::cout << "Adjusted by +2: Position = " << second_hit.position << std::endl;
                second_hit.position += 1;
                std::cout << "Adjusted by +3: Position = " << second_hit.position << std::endl;
                break;
            case 4: // 100
                second_hit.position += 1;
                std::cout << "Adjusted by +1: Position = " << second_hit.position << std::endl;
                break;
            case 5: // 101
                second_hit.position += 1;
                std::cout << "Adjusted by +1: Position = " << second_hit.position << std::endl;
                second_hit.position += 3;
                std::cout << "Adjusted by +3: Position = " << second_hit.position << std::endl;
                break;
            case 6: // 110
                second_hit.position += 1;
                std::cout << "Adjusted by +1: Position = " << second_hit.position << std::endl;
                second_hit.position += 2;
                std::cout << "Adjusted by +2: Position = " << second_hit.position << std::endl;
                break;
            case 7: // 111
                second_hit.position += 1;
                std::cout << "Adjusted by +1: Position = " << second_hit.position << std::endl;
                second_hit.position += 2;
                std::cout << "Adjusted by +2: Position = " << second_hit.position << std::endl;
                second_hit.position += 3;
                std::cout << "Adjusted by +3: Position = " << second_hit.position << std::endl;
                break;
            default:
                // No action on default
                break;
        }

        // Check adjacency
        bool isAdjacent = (first_hit.ABCStarID == second_hit.ABCStarID && second_hit.position == first_hit.position + 1) ||
                          (first_hit.ABCStarID + 1 == second_hit.ABCStarID && first_hit.position == ABCStar_SIZE - 1 && second_hit.position == 0);
        std::cout << "Adjacency check: " << isAdjacent << std::endl;

        if (!isAdjacent && !init) {
            // Output the cluster
            OutputData outputData;
            outputData.data = (first_hit.ABCStarID << (POSITION_BITS + SIZE_BITS)) | (first_hit.position << SIZE_BITS) | clusterSize;
            outputData.last = 0;
            outputClustersStream.write(outputData);
            std::cout << "Output cluster: ABCStarID = " << (outputData.data >> (POSITION_BITS + SIZE_BITS))
                      << ", Position = " << ((outputData.data >> SIZE_BITS) & ((1 << POSITION_BITS) - 1))
                      << ", Size = " << (outputData.data & ((1 << SIZE_BITS) - 1))
                      << ", Last = " << outputData.last << std::endl;
            clusterSize = 1;
        } else {
            clusterSize++;
        }

        init = false;
        if (last) {
            OutputData outputData;
            outputData.data = (second_hit.ABCStarID << (POSITION_BITS + SIZE_BITS)) | (second_hit.position << SIZE_BITS) | clusterSize;
            outputData.last = 1;
            outputClustersStream.write(outputData);
            std::cout << "Final cluster output: ABCStarID = " << (outputData.data >> (POSITION_BITS + SIZE_BITS))
                      << ", Position = " << ((outputData.data >> SIZE_BITS) & ((1 << POSITION_BITS) - 1))
                      << ", Size = " << (outputData.data & ((1 << SIZE_BITS) - 1))
                      << ", Last = " << outputData.last << std::endl;
        }
    }
}
