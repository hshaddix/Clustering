#include "processHits.h"

// Checks if two hits are adjacent considering the size of the cluster
bool areAdjacent(const Hit &first_hit, const Hit &second_hit, int size) {
    return (first_hit.ABCStarID == second_hit.ABCStarID && (first_hit.position + size == second_hit.position)) ||
           (first_hit.ABCStarID + 1 == second_hit.ABCStarID && first_hit.position == ABCStar_SIZE - 1 && second_hit.position == 0);
}

// Outputs a cluster to the stream
void outputCluster(const Hit &hit, int size, hls::stream<OutputData> &stream, bool isLast = false) {
    OutputData outputData;
    outputData.data = (hit.ABCStarID << (POSITION_BITS + SIZE_BITS)) | (hit.position << SIZE_BITS) | size;
    outputData.last = isLast ? 1 : 0;
    stream.write(outputData);
    std::cout << "Output cluster: ABCStarID = " << hit.ABCStarID << ", Position = " << hit.position << ", Size = " << size << ", Last = " << outputData.last << std::endl;
}

// Function to process hits and cluster them
void processHits(hls::stream<InputData> &inputBinariesStream, hls::stream<OutputData> &outputClustersStream) {
    #pragma HLS INTERFACE s_axilite port=return
    #pragma HLS INTERFACE axis port=inputBinariesStream
    #pragma HLS INTERFACE axis port=outputClustersStream

    Hit first_hit, second_hit;
    bool init = true, last = false;
    int clusterSize = 0;

    while (!inputBinariesStream.empty()) {
        InputData inputData = inputBinariesStream.read();
        last = inputData.last;

        if (!init) {
            first_hit = second_hit;  // Set first_hit for comparison
        }

        second_hit.ABCStarID = inputData.data.range(15, 11);
        int basePosition = inputData.data.range(10, 3);
        ap_uint<3> sizeBitmask = inputData.data.range(2, 0);

        std::cout << "Read Data: ABCStarID = " << second_hit.ABCStarID << ", Base Position = " << basePosition << ", Bitmask = " << sizeBitmask << std::endl;

        clusterSize = 0;
        switch(sizeBitmask.to_uint()) {
            case 1: // 001
                second_hit.position = basePosition + 1;
                clusterSize = 1;
                break;
            case 2: // 010
                second_hit.position = basePosition + 2;
                clusterSize = 1;
                break;
            case 3: // 011
                second_hit.position = basePosition + 2;
                if (!init && !areAdjacent(first_hit, second_hit, clusterSize)) {
                    outputCluster(first_hit, clusterSize, outputClustersStream);
                }
                clusterSize = 1; // Reset for next hit
                second_hit.position = basePosition + 3;
                break;
            case 4: // 100
                second_hit.position = basePosition + 3;
                clusterSize = 1;
                break;
            case 5: // 101
                second_hit.position = basePosition + 1;
                if (!init && !areAdjacent(first_hit, second_hit, clusterSize)) {
                    outputCluster(first_hit, clusterSize, outputClustersStream);
                }
                clusterSize = 1; // Reset for next hit
                second_hit.position = basePosition + 3;
                break;
            case 6: // 110
                second_hit.position = basePosition + 1;
                if (!init && !areAdjacent(first_hit, second_hit, clusterSize)) {
                    outputCluster(first_hit, clusterSize, outputClustersStream);
                }
                clusterSize = 1; // Reset for next hit
                second_hit.position = basePosition + 2;
                break;
            case 7: // 111
                second_hit.position = basePosition + 1;
                if (!init && !areAdjacent(first_hit, second_hit, clusterSize)) {
                    outputCluster(first_hit, clusterSize, outputClustersStream);
                }
                clusterSize = 1; // Reset for next hit
                second_hit.position = basePosition + 2;
                if (!init && !areAdjacent(first_hit, second_hit, clusterSize)) {
                    outputCluster(first_hit, clusterSize, outputClustersStream);
                }
                clusterSize = 1; // Reset for next hit
                second_hit.position = basePosition + 3;
                break;
            default:
                // No action on default
                break;
        }

        if (!init && !areAdjacent(first_hit, second_hit, clusterSize)) {
            outputCluster(first_hit, clusterSize, outputClustersStream);
            clusterSize = 1; // Reset cluster size for new hit
        } else {
            clusterSize++;
        }

        first_hit = second_hit; // Move second_hit to first_hit for next iteration
        init = false;

        if (last) {
            outputCluster(second_hit, clusterSize, outputClustersStream, true);
        }
    }
}
