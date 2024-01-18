#include <iostream>
#include <vector>
#include <bitset>
#include <algorithm>
#include <string>

const int STRIP_SIZE = 126;

struct Cluster {
    int stripNumber;
    int startPosition;
    int size;
    int globalStart;
    int globalEnd;
};

Cluster parseCluster(const std::string& binary) {
    std::bitset<16> bits(binary);
    int stripNumber = (bits >> 11).to_ulong();
    int startPosition = ((bits << 5) >> 8).to_ulong();
    int size = ((bits << 13) >> 13).to_ulong() + 1;

    // Calculate global positions with offset
    int globalStart = stripNumber * STRIP_SIZE + startPosition;
    int globalEnd = globalStart + size - 1;

    return {stripNumber, startPosition, size, globalStart, globalEnd};
}

bool areAdjacent(const Cluster& a, const Cluster& b) {
    // Adjust to handle adjacency at the strip boundary
    if (a.stripNumber != b.stripNumber) {
        return (a.globalEnd + 1) % STRIP_SIZE == b.startPosition;
    }
    return a.globalEnd + 1 == b.globalStart;
}

std::vector<Cluster> mergeClusters(std::vector<Cluster>& clusters) {
    if (clusters.empty()) return {};

    std::sort(clusters.begin(), clusters.end(), [](const Cluster& a, const Cluster& b) {
        return a.globalStart < b.globalStart;
    });

    std::vector<Cluster> merged;
    Cluster current = clusters[0];
    int offset = 0;  // Initialize the offset

    for (size_t i = 1; i < clusters.size(); ++i) {
        if (areAdjacent(current, clusters[i])) {
            // Merge clusters and sum their sizes
            current.size += clusters[i].size;
            current.globalEnd = current.globalStart + current.size - 1;
        } else {
            merged.push_back(current);

            // Adjust the global positions with the offset
            current.globalStart += offset * STRIP_SIZE;
            current.globalEnd += offset * STRIP_SIZE;

            // Check if clusters span multiple strips
            if (current.globalEnd >= STRIP_SIZE) {
                // Calculate the new offset
                offset += 1;

                // Adjust the global positions again
                current.globalStart -= STRIP_SIZE;
                current.globalEnd -= STRIP_SIZE;
            }

            current = clusters[i];
        }
    }

    // Add the last cluster
    merged.push_back(current);

    // Adjust the global positions of the last cluster with the final offset
    current.globalStart += offset * STRIP_SIZE;
    current.globalEnd += offset * STRIP_SIZE;

    return merged;
}

std::string toBinaryString(const Cluster& cluster) {
    std::bitset<4> binaryStrip(cluster.stripNumber);
    std::bitset<8> binaryStart(cluster.startPosition);
    std::bitset<3> binarySize(cluster.size - 1);

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
