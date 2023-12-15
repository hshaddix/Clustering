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
    int startPosition = ((bits << 4) >> 8).to_ulong();
    int size = (bits & 0b111).to_ulong() + 1;  // Size is 1-based

    return {stripNumber, startPosition, size};
}

std::vector<Cluster> mergeClusters(const std::vector<Cluster>& clusters) {
    if (clusters.empty()) return {};

    std::vector<Cluster> merged;
    Cluster current = clusters[0];

    for (size_t i = 1; i < clusters.size(); ++i) {
        int globalEnd = current.stripNumber * STRIP_SIZE + current.startPosition + current.size - 1;
        int globalStartNext = clusters[i].stripNumber * STRIP_SIZE + clusters[i].startPosition;

        if (globalEnd >= globalStartNext) {
            int newEnd = clusters[i].stripNumber * STRIP_SIZE + clusters[i].startPosition + clusters[i].size - 1;
            current.size = newEnd - (current.stripNumber * STRIP_SIZE + current.startPosition) + 1;
        } else {
            merged.push_back(current);
            current = clusters[i];
        }
    }

    merged.push_back(current);
    return merged;
}

int main() {
    std::string inputBinary;
    std::vector<Cluster> clusters;

    while (std::cin >> inputBinary) {
        clusters.push_back(parseCluster(inputBinary));
    }

    std::sort(clusters.begin(), clusters.end(), [](const Cluster& a, const Cluster& b) { 
        return a.stripNumber < b.stripNumber || (a.stripNumber == b.stripNumber && a.startPosition < b.startPosition); 
    });

    auto mergedClusters = mergeClusters(clusters);
    for (const auto& cluster : mergedClusters) {
        std::cout << "Strip " << cluster.stripNumber << ": Start " << cluster.startPosition << ", Size " << cluster.size << std::endl;
    }

    return 0;
}
