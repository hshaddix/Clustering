#include <iostream>
#include <vector>
#include <bitset>
#include <algorithm>
#include <string>

const int STRIP_SIZE = 126; // Number of channels in ABC*

struct Cluster {
    int stripNumber;   // ABCstar ID/number
    int startPosition; // Starting position of the cluster on the strip
    int size;          // Size of the cluster as a bitmask
    int globalStart;   // Global start position of the cluster
    int globalEnd;     // Global end position of the cluster, adjusted for bitmask size
};

// Function to decode the size bitmask into the actual number of hits
int decodeSize(const std::bitset<3>& sizeBitmask) {
    // Convert size bitmask to integer
    int size = static_cast<int>(sizeBitmask.to_ulong());
    // The number of hits is the value of the bitmask (e.g., "101" is 3 extra hits)
    return size; // Adjust this logic if the interpretation of size changes
}

Cluster parseCluster(const std::string& binary) {
    std::bitset<16> bits(binary);
    int stripNumber = (bits >> 11).to_ulong();
    int startPosition = ((bits << 5) >> 8).to_ulong();
    // Decode size from the last 3 bits
    int size = decodeSize(bits.to_ulong() & 0x7); // Last 3 bits as size bitmask

    int globalStart = stripNumber * STRIP_SIZE + startPosition;
    // Adjust globalEnd calculation based on decoded size
    int globalEnd = globalStart + size;

    return {stripNumber, startPosition, size, globalStart, globalEnd};
}

// Adjacency check and mergeClusters function remain unchanged

std::string toBinaryString(const Cluster& cluster) {
    std::bitset<4> binaryStrip(cluster.stripNumber);
    std::bitset<8> binaryStart(cluster.startPosition);
    std::bitset<3> binarySize(cluster.size); // Assuming size encoding remains as is

    return "0" + binaryStrip.to_string() + binaryStart.to_string() + binarySize.to_string();
}

int main() {
    std::string binary;
    std::vector<Cluster> clusters;

    while (std::cin >> binary) {
        clusters.push_back(parseCluster(binary));
    }

    auto mergedClusters = mergeClusters(clusters);
    for (const auto& cluster : mergedClusters) {
        std::cout << toBinaryString(cluster) << std::endl;
    }

    return 0;
}
