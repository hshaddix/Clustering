#include <iostream>
#include <vector>
#include <bitset>
#include <algorithm>
#include <string>

const int STRIP_SIZE = 126;

struct Cluster {
    int globalStart;
    int globalEnd;
};

Cluster parseCluster(const std::string& binary) {
    std::bitset<16> bits(binary);
    int stripNumber = (bits >> 11).to_ulong();
    int startPosition = ((bits << 5) >> 8).to_ulong();
    int size = ((bits << 13) >> 13).to_ulong() + 1;

    int globalStart = (stripNumber - 1) * STRIP_SIZE + startPosition;
    int globalEnd = globalStart + size - 1;

    return {globalStart, globalEnd};
}

std::vector<Cluster> mergeClusters(std::vector<Cluster>& clusters) {
    std::sort(clusters.begin(), clusters.end(), [](const Cluster& a, const Cluster& b) {
        return a.globalStart < b.globalStart;
    });

    std::vector<Cluster> merged;
    for (const auto& cluster : clusters) {
        if (!merged.empty() && merged.back().globalEnd + 1 >= cluster.globalStart) {
            merged.back().globalEnd = std::max(merged.back().globalEnd, cluster.globalEnd);
        } else {
            merged.push_back(cluster);
        }
    }

    return merged;
}

std::string toBinaryString(const Cluster& cluster) {
    int stripNumber = cluster.globalStart / STRIP_SIZE + 1;
    int startPosition = cluster.globalStart % STRIP_SIZE;
    int size = cluster.globalEnd - cluster.globalStart + 1;

    std::bitset<4> binaryStrip(stripNumber);
    std::bitset<8> binaryStart(startPosition);
    std::bitset<3> binarySize(size - 1);

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
