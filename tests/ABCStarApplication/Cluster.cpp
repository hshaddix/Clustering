#include <iostream>
#include <vector>
#include <bitset>
#include <algorithm>
#include <string>

const int STRIP_SIZE = 126; // Number of channels in ABC*

struct Cluster {
    int stripNumber;   // ABCstar ID/number
    int startPosition; // Starting position of the cluster on the strip
    int size;          // Size of the cluster, decoded from bitmask
    int globalStart;   // Global start position of the cluster
    int globalEnd;     // Global end position of the cluster
};

// Decode the size bitmask into actual number of hits
int decodeSize(int bitmask) {
    switch (bitmask) {
        case 0: return 1; // 000 represents 1 hit
        case 1: return 2; // 001 represents 2 hits
        case 2: return 3; // 010 represents 3 hits, and so on...
        case 3: return 4;
        case 4: return 5;
        case 5: return 6;
        case 6: return 7;
        case 7: return 8;
        default: return 1; // Default to 1 if unexpected value
    }
}

Cluster parseCluster(const std::string& binary) {
    std::bitset<16> bits(binary);
    int stripNumber = (bits >> 11).to_ulong();
    int startPosition = ((bits << 5) >> 8).to_ulong();
    int sizeBitmask = ((bits << 13) >> 13).to_ulong();
    int size = decodeSize(sizeBitmask); // Decode size from the bitmask

    int globalStart = stripNumber * STRIP_SIZE + startPosition;
    int globalEnd = globalStart + size - 1; // Adjusted as per feedback

    return {stripNumber, startPosition, size, globalStart, globalEnd};
};

// Remaining functions (areAdjacent, mergeClusters, toBinaryString) are unchanged

int main() {
    std::string binary; // String to store each line of binary input
    std::vector<Cluster> clusters; // Vector to store the parsed clusters

    while (std::cin >> binary) {
        clusters.push_back(parseCluster(binary));
    }

    auto mergedClusters = mergeClusters(clusters);
    for (const auto& cluster : mergedClusters) {
        std::cout << toBinaryString(cluster) << std::endl;
    }

    return 0;
}
