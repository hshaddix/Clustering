#include <iostream>
#include "test.cpp" // Include the core functionality

#ifndef __SYNTHESIS__
// Testbench
int main() {
    hls::stream<InputData> testDataStream;
    ClusterInfo testOutputClusters[MAX_CLUSTERS];
    int testOutputClusterCount = 0;

    // Case 1
    InputData inputDataCase1 = {0b000000110010010};
    testDataStream.write(inputDataCase1);
    processHits(testDataStream, 1, testOutputClusters, testOutputClusterCount);
    std::cout << "Case 1 Output Cluster Count: " << testOutputClusterCount << std::endl;

    // Reset stream for the next case
    testDataStream = {};

    // Case 2
    InputData inputDataCase2_1 = {0b000000110010101};
    InputData inputDataCase2_2 = {0b000000110110100};
    testDataStream.write(inputDataCase2_1);
    testDataStream.write(inputDataCase2_2);
    processHits(testDataStream, 2, testOutputClusters, testOutputClusterCount);
    std::cout << "Case 2 Output Cluster Count: " << testOutputClusterCount << std::endl;

    // Reset stream for the next case
    testDataStream = {};

    // Case 3
    InputData inputDataCase3_1 = {0b0000001111010111};
    InputData inputDataCase3_2 = {0b0000100000000110};
    testDataStream.write(inputDataCase3_1);
    testDataStream.write(inputDataCase3_2);
    processHits(testDataStream, 2, testOutputClusters, testOutputClusterCount);
    std::cout << "Case 3 Output Cluster Count: " << testOutputClusterCount << std::endl;

    return 0;
}
#endif
