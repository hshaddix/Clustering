#include <iostream>
#include "test.cpp" // Make sure this path is correctly set to where processHits.cpp is located

int main() {
    ClusterInfo testOutputClusters[MAX_CLUSTERS];
    int testOutputClusterCount;

    // Case 1
    std::cout << "Running case 1:" << std::endl;
    {
        hls::stream<InputData> testDataStream;
        testOutputClusterCount = 0;
        InputData inputDataCase1 = {0b000000110010010};
        testDataStream.write(inputDataCase1);
        processHits(testDataStream, 1, testOutputClusters, &testOutputClusterCount);
        std::cout << "Case 1 Output Cluster Count: " << testOutputClusterCount << std::endl;
    }

    // Case 2
    std::cout << "\nRunning case 2:" << std::endl;
    {
        hls::stream<InputData> testDataStream;
        testOutputClusterCount = 0;
        InputData inputDataCase2_1 = {0b000000110010101};
        InputData inputDataCase2_2 = {0b000000110110100};
        testDataStream.write(inputDataCase2_1);
        testDataStream.write(inputDataCase2_2);
        processHits(testDataStream, 2, testOutputClusters, &testOutputClusterCount);
        std::cout << "Case 2 Output Cluster Count: " << testOutputClusterCount << std::endl;
    }

    // Case 3
    std::cout << "\nRunning case 3:" << std::endl;
    {
        hls::stream<InputData> testDataStream;
        testOutputClusterCount = 0;
        InputData inputDataCase3_1 = {0b0000001111010111};
        InputData inputDataCase3_2 = {0b0000100000000110};
        testDataStream.write(inputDataCase3_1);
        testDataStream.write(inputDataCase3_2);
        processHits(testDataStream, 2, testOutputClusters, &testOutputClusterCount);
        std::cout << "Case 3 Output Cluster Count: " << testOutputClusterCount << std::endl;
    }

    return 0;
}
