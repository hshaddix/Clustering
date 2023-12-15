#include <iostream>
#include <vector>
#include <bitset>
#include <algorithm>
#include <string>
#include <set>

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

std::vector<Cluster> mergeClusters(std::vector<Cluster>& clusters) {
    std::set<int> hits;

    for (const auto& cluster : clusters) {
        int globalStart = (cluster.stripNumber - 1) * STRIP_SIZE + cluster.startPosition;
        for (int i = 0; i < cluster.size; ++i) {
            hits.insert(globalStart + i);
        }
    }

    std::vector<Cluster> merged;
    int currentStrip = 0, currentStart = -1, currentSize = 0;

    for (int hit : hits) {
        int strip = hit / STRIP_SIZE + 1;
        int position = hit % STRIP_SIZE;

        if (currentStart == -1) {
            currentStrip = strip;
            currentStart = position;
            currentSize = 1;
        } else if (strip == currentStrip && position == currentStart + currentSize) {
            currentSize++;
        } else {
            merged.push_back({currentStrip, currentStart, currentSize});
            currentStrip = strip;
            currentStart = position;
            currentSize = 1;
        }
    }

    if (currentSize > 0) {
        merged.push_back({currentStrip, currentStart, currentSize});
    }

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
