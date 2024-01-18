#include <iostream>
#include <vector>
#include <bitset>
#include <string>

const int STRIP_SIZE = 126;

struct Cluster {
    int stripNumber;
    int startPosition;
    int size;

    int globalStart() const {
        return stripNumber * STRIP_SIZE + startPosition;
    }
};

Cluster parseCluster(const std::string& binary) {
    std::bitset<16> bits(binary);
    Cluster c;
    c.stripNumber = (bits >> 12).to_ulong() & 0xF;     // Extracting bits 2-5
    c.startPosition = (bits >> 4).to_ulong() & 0xFF;   // Extracting bits 6-13
    c.size = (bits.to_ulong() & 0xF) + 1;              // Extracting bits 14-16 and adjusting size
    return c;
}

bool areAdjacent(const Cluster& a, const Cluster& b) {
    return a.globalStart() + a.size == b.globalStart();
}

std::vector<Cluster> mergeClusters(std::vector<Cluster>& clusters) {
    std::vector<Cluster> merged;
    if (clusters.empty()) return merged;

    Cluster current = clusters[0];
    for (size_t i = 1; i < clusters.size(); ++i) {
        if (areAdjacent(current, clusters[i])) {
            current.size += clusters[i].size;
        } else {
            merged.push_back(current);
            current = clusters[i];
        }
    }
    merged.push_back(current);
    return merged;
}

std::string toBinaryString(const Cluster& cluster) {
    std::bitset<4> strip(cluster.stripNumber);
    std::bitset<8> start(cluster.startPosition);
    std::bitset<3> size(cluster.size - 1);
    return "0" + strip.to_string() + start.to_string() + size.to_string();
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
