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
    int size = ((bits << 13) >> 13).to_ulong() + 1; // Size is 1-based

    int globalStart = (stripNumber - 1) * STRIP_SIZE + startPosition;
    int globalEnd = globalStart + size - 1;

    return {stripNumber, startPosition, size, globalStart, globalEnd};
}

std::vector<Cluster> mergeClusters(std::vector<Cluster>& clusters) {
    if (clusters.empty()) return {};

    std::sort(clusters.begin(), clusters.end(), [](const Cluster& a, const Cluster& b) {
        return a.globalStart < b.globalStart;
    });

    std::vector<Cluster> merged;
    Cluster current = clusters[0];

    for (size_t i = 1; i < clusters.size(); ++i) {
        if (current.globalEnd + 1 == clusters[i].globalStart) {
            // Merge adjacent clusters
            current.globalEnd = clusters[i].globalEnd;
            current.size = current.globalEnd - current.globalStart + 1;
        } else {
            merged.push_back(current);
            current = clusters[i];
        }
    }

    merged.push_back(current);
    return merged;
}

std::string toBinaryString(const Cluster& cluster) {
    std::bitset<4> binaryStrip(cluster.stripNumber);
    std::bitset<8> binaryStart(cluster.startPosition);
    std::bitset<3> binarySize(cluster.size - 1); // Size in binary is actual size - 1

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
