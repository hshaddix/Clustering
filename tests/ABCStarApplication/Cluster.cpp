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
    int size = static_cast<int>(sizeBitmask.to_ulong());
    return size; // Decodes size bitmask to integer
}

// Function to check if two clusters are adjacent
bool areAdjacent(const Cluster& a, const Cluster& b) {
    if (a.stripNumber != b.stripNumber) {
        return (a.globalEnd + 1) % STRIP_SIZE == b.startPosition;
    }
    return a.globalEnd + 1 == b.globalStart;
}

// Function to merge adjacent clusters into a single cluster
std::vector<Cluster> mergeClusters(std::vector<Cluster>& clusters) {
    if (clusters.empty()) return {};

    std::sort(clusters.begin(), clusters.end(), [](const Cluster& a, const Cluster& b) {
        return a.globalStart < b.globalStart;
    });

    std::vector<Cluster> merged;
    Cluster current = clusters[0];

    for (size_t i = 1; i < clusters.size(); ++i) {
        if (areAdjacent(current, clusters[i])) {
            current.size += clusters[i].size;
            current.globalEnd = current.globalStart + current.size - 1;
        } else {
            merged.push_back(current);
            current = clusters[i];
        }
    }

    merged.push_back(current);
    return merged;
}

Cluster parseCluster(const std::string& binary) {
    std::bitset<16> bits(binary);
    int stripNumber = (bits >> 11).to_ulong();
    int startPosition = ((bits << 5) >> 8).to_ulong();
    int size = decodeSize(bits.to_ulong() & 0x7);

    int globalStart = stripNumber * STRIP_SIZE + startPosition;
    int globalEnd = globalStart + size;

    return {stripNumber, startPosition, size, globalStart, globalEnd};
}

std::string toBinaryString(const Cluster& cluster) {
    std::bitset<4> binaryStrip(cluster.stripNumber);
    std::bitset<8> binaryStart(cluster.startPosition);
    std::bitset<3> binarySize(cluster.size);

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
