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
};

Cluster parseCluster(const std::string& binary) {
    std::bitset<16> bits(binary);
    int stripNumber = (bits >> 11).to_ulong();
    int startPosition = ((bits << 5) >> 8).to_ulong();
    int size = ((bits << 13) >> 13).to_ulong() + 1; // Size is 1-based

    return {stripNumber, startPosition, size};
}

std::string toBinaryString(const Cluster& cluster) {
    std::bitset<4> stripNumber(cluster.stripNumber);
    std::bitset<8> startPosition(cluster.startPosition);
    std::bitset<3> size(cluster.size - 1); // Size in binary is actual size - 1

    return "0" + stripNumber.to_string() + startPosition.to_string() + size.to_string();
}

std::vector<Cluster> mergeClusters(std::vector<Cluster> clusters) {
    if (clusters.empty()) return {};

    // Sort clusters by global start position
    std::sort(clusters.begin(), clusters.end(), [](const Cluster& a, const Cluster& b) {
        return a.stripNumber * STRIP_SIZE + a.startPosition < b.stripNumber * STRIP_SIZE + b.startPosition;
    });

    std::vector<Cluster> merged;
    Cluster current = clusters[0];

    for (size_t i = 1; i < clusters.size(); ++i) {
        int currentEnd = current.stripNumber * STRIP_SIZE + current.startPosition + current.size;
        int nextStart = clusters[i].stripNumber * STRIP_SIZE + clusters[i].startPosition;

        if (currentEnd >= nextStart) {
            // Extend the current cluster to include the next one
            int newEnd = clusters[i].stripNumber * STRIP_SIZE + clusters[i].startPosition + clusters[i].size;
            current.size = newEnd - (current.stripNumber * STRIP_SIZE + current.startPosition);
        } else {
            merged.push_back(current);
            current = clusters[i];
        }
    }

    merged.push_back(current);
    return merged;
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
