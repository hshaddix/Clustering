#include <iostream>
#include <ap_int.h>
#include <hls_stream.h>
#include "processHits.h"

#ifndef __SYNTHESIS__
// Testbench
int main() {
    ClusterInfo testOutputClusters[MAX_CLUSTERS];
    int testOutputClusterCount;

    // Case 1
    {
        hls::stream<InputData> testDataStream;
        InputData inputDataCase1 = {0b0000000110010010};
        testDataStream.write(inputDataCase1);
        processHits(testDataStream, 1, testOutputClusters, testOutputClusterCount);
        std::cout << "Case 1 Output Cluster Count: " << testOutputClusterCount << std::endl;
        for (int i = 0; i < testOutputClusterCount; ++i) {
            std::cout << "Cluster " << i+1 << ": Position (ABCStarID: " << testOutputClusters[i].firstHit.ABCStarID << ", Position: " << testOutputClusters[i].firstHit.position << "), Size: " << testOutputClusters[i].size << std::endl;
        }
    }

    // Case 2
    {
        hls::stream<InputData> testDataStream;
        InputData inputDataCase2_1 = {0b000000110010101};
        InputData inputDataCase2_2 = {0b000000110110100};
        testDataStream.write(inputDataCase2_1);
        testDataStream.write(inputDataCase2_2);
        processHits(testDataStream, 2, testOutputClusters, testOutputClusterCount);
        std::cout << "Case 2 Output Cluster Count: " << testOutputClusterCount << std::endl;
        for (int i = 0; i < testOutputClusterCount; ++i) {
            std::cout << "Cluster " << i+1 << ": Position (ABCStarID: " << testOutputClusters[i].firstHit.ABCStarID << ", Position: " << testOutputClusters[i].firstHit.position << "), Size: " << testOutputClusters[i].size << std::endl;
        }
    }

    // Case 3
    {
        hls::stream<InputData> testDataStream;
        InputData inputDataCase3_1 = {0b0000001111010111};
        InputData inputDataCase3_2 = {0b0000100000000110};
        testDataStream.write(inputDataCase3_1);
        testDataStream.write(inputDataCase3_2);
        processHits(testDataStream, 2, testOutputClusters, testOutputClusterCount);
        std::cout << "Case 3 Output Cluster Count: " << testOutputClusterCount << std::endl;
        for (int i = 0; i < testOutputClusterCount; ++i) {
            std::cout << "Cluster " << i+1 << ": Position (ABCStarID: " << testOutputClusters[i].firstHit.ABCStarID << ", Position: " << testOutputClusters[i].firstHit.position << "), Size: " << testOutputClusters[i].size << std::endl;
        }
    }

    return 0;
}
#endif
