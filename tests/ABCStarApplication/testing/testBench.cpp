#include <iostream>
#include <ap_int.h>
#include <hls_stream.h>
#include "processHits.h"
#include <bitset>

#ifndef __SYNTHESIS__
// Testbench
int main() {
    // Declare the output stream to hold cluster information
    hls::stream<OutputData> outputClustersStream;

    // Define inputs for each case
    int id1 = 0, position1 = 50;
    unsigned char bitmask1 = 0b111;

    int id2 = 0, position2a = 50, position2b = 54;
    unsigned char bitmask2a = 0b110, bitmask2b = 0b100;

    int id3a = 0, position3a = 126;
    int id3b = 1, position3b = 0;
    unsigned char bitmask3 = 0b100;

    // Case 1: ID = 0, Position = 50, Bitmask = 010
    {
        hls::stream<InputData> testDataStream;
        std::cout << "Case 1 Input: ABCStarID = " << id1 << ", Position = " << position1 << ", Bitmask = " << std::bitset<3>(bitmask1) << std::endl;
        unsigned short data1 = (id1 << 11) | (position1 << 3) | bitmask1;
        InputData inputDataCase1 = {data1, false}; // Not the last data
        testDataStream.write(inputDataCase1);

        processHits(testDataStream, outputClustersStream);

        std::cout << "Case 1 Results:" << std::endl;
        while (!outputClustersStream.empty()) {
            OutputData output = outputClustersStream.read();
            std::cout << "Cluster: ABCStarID = " << (output.data >> (POSITION_BITS + SIZE_BITS))
                      << ", Position = " << ((output.data >> SIZE_BITS) & ((1 << POSITION_BITS) - 1))
                      << ", Size = " << (output.data & ((1 << SIZE_BITS) - 1))
                      << ", Last = " << output.last << std::endl;
        }
    }

    // Case 2: Two clusters on the same ABCStar with positions 50 and 53
    {
        hls::stream<InputData> testDataStream;
        std::cout << "Case 2 Input: \n";
        std::cout << "    ABCStarID = " << id2 << ", Position = " << position2a << ", Bitmask = " << std::bitset<3>(bitmask2a) << std::endl;
        std::cout << "    ABCStarID = " << id2 << ", Position = " << position2b << ", Bitmask = " << std::bitset<3>(bitmask2b) << std::endl;
        InputData inputData1 = {(id2 << 11) | (position2a << 3) | bitmask2a, false}; // Not the last data
        InputData inputData2 = {(id2 << 11) | (position2b << 3) | bitmask2b, true};  // Last data
        testDataStream.write(inputData1);
        testDataStream.write(inputData2);

        processHits(testDataStream, outputClustersStream);

        std::cout << "Case 2 Results:" << std::endl;
        while (!outputClustersStream.empty()) {
            OutputData output = outputClustersStream.read();
            std::cout << "Cluster: ABCStarID = " << (output.data >> (POSITION_BITS + SIZE_BITS))
                      << ", Position = " << ((output.data >> SIZE_BITS) & ((1 << POSITION_BITS) - 1))
                      << ", Size = " << (output.data & ((1 << SIZE_BITS) - 1))
                      << ", Last = " << output.last << std::endl;
        }
    }

    // Case 3: Two clusters on different ABCStar with positions 126 and 0
    {
        hls::stream<InputData> testDataStream;
        std::cout << "Case 3 Input: \n";
        std::cout << "    ABCStarID = " << id3a << ", Position = " << position3a << ", Bitmask = " << std::bitset<3>(bitmask3) << std::endl;
        std::cout << "    ABCStarID = " << id3b << ", Position = " << position3b << ", Bitmask = " << std::bitset<3>(bitmask3) << std::endl;
        InputData inputData1 = {(id3a << 11) | (position3a << 3) | bitmask3, false}; // Not the last data
        InputData inputData2 = {(id3b << 11) | (position3b << 3) | bitmask3, true};  // Last data
        testDataStream.write(inputData1);
        testDataStream.write(inputData2);

        processHits(testDataStream, outputClustersStream);

        std::cout << "Case 3 Results:" << std::endl;
        while (!outputClustersStream.empty()) {
            OutputData output = outputClustersStream.read();
            std::cout << "Cluster: ABCStarID = " << (output.data >> (POSITION_BITS + SIZE_BITS))
                      << ", Position = " << ((output.data >> SIZE_BITS) & ((1 << POSITION_BITS) - 1))
                      << ", Size = " << (output.data & ((1 << SIZE_BITS) - 1))
                      << ", Last = " << output.last << std::endl;
        }
    }

    return 0;
}
#endif
