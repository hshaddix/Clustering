#include <iostream>
#include <ap_int.h>
#include <hls_stream.h>
#include "processHits.h"

#ifndef __SYNTHESIS__
// Testbench
int main() {
    // Declare the output stream to hold cluster information
    hls::stream<OutputData> outputClustersStream;

    // Case 1: ID = 0, Position = 50, Bitmask = 010
    {
        hls::stream<InputData> testDataStream;
        unsigned short data1 = (0 << 11) | (50 << 3) | 0b010;
        InputData inputDataCase1 = {data1, false}; // Not the last data
        testDataStream.write(inputDataCase1);

        processHits(testDataStream, outputClustersStream);

        std::cout << "Case 1 Results:" << std::endl;
        while (!outputClustersStream.empty()) {
            OutputData output = outputClustersStream.read();
            std::cout << "Cluster: Position = " << (output.data >> 4)
                      << ", Size = " << (output.data & 0xF)
                      << ", Last = " << output.last << std::endl;
        }
    }

    // Case 2: Two clusters on the same ABCStar with positions 50 and 53
    {
        hls::stream<InputData> testDataStream;
        InputData inputData1 = {(0 << 11) | (50 << 3) | 0b101, false}; // Not the last data
        InputData inputData2 = {(0 << 11) | (53 << 3) | 0b110, true};  // Last data
        testDataStream.write(inputData1);
        testDataStream.write(inputData2);

        processHits(testDataStream, outputClustersStream);

        std::cout << "Case 2 Results:" << std::endl;
        while (!outputClustersStream.empty()) {
            OutputData output = outputClustersStream.read();
            std::cout << "Cluster: Position = " << (output.data >> 4)
                      << ", Size = " << (output.data & 0xF)
                      << ", Last = " << output.last << std::endl;
        }
    }

      // Case 3: Two clusters on different ABCStar with positions 126 and 0
    {
        hls::stream<InputData> testDataStream;
        InputData inputData1 = {(0 << 11) | (126 << 3) | 0b100, false}; // Not the last data
        InputData inputData2 = {(1 << 11) | (0 << 3) | 0b100, true};  // Last data
        testDataStream.write(inputData1);
        testDataStream.write(inputData2);

        processHits(testDataStream, outputClustersStream);

        std::cout << "Case 3 Results:" << std::endl;
        while (!outputClustersStream.empty()) {
            OutputData output = outputClustersStream.read();
            std::cout << "Cluster: Position = " << (output.data >> 4)
                      << ", Size = " << (output.data & 0xF)
                      << ", Last = " << output.last << std::endl;
        }
    }

    // Additional cases can be defined in a similar manner

    return 0;
}
#endif
