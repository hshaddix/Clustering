#include <iostream>
#include <vector>
#include <bitset>
#include <algorithm>

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
    int size = ((bits << 13) >> 13).to_ulong() + 1;  // Size is 1-based

    return {stripNumber, startPosition, size};
}

std::vector<Cluster> mergeClusters(std::vector<Cluster> clusters) {
    std::sort(clusters.begin(), clusters.end(), [](const Cluster& a, const Cluster& b) {
        return a.stripNumber < b.stripNumber || (a.stripNumber == b.stripNumber && a.startPosition < b.startPosition);
    });

    std::vector<Cluster> merged;
    for (const auto& c : clusters) {
        if (!merged.empty()) {
            auto& last = merged.back();
            int lastGlobalEnd = last.stripNumber * STRIP_SIZE + last.startPosition + last.size - 1;
            int currentGlobalStart = c.stripNumber * STRIP_SIZE + c.startPosition;

            if (lastGlobalEnd >= currentGlobalStart) {
                int newEnd = c.stripNumber * STRIP_SIZE + c.startPosition + c.size - 1;
                last.size = newEnd - (last.stripNumber * STRIP_SIZE + last.startPosition) + 1;
                continue;
            }
        }
        merged.push_back(c);
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
        std::cout << "Strip " << cluster.stripNumber << ": Start " << cluster.startPosition << ", Size " << cluster.size << std::endl;
    }

    return 0;
}
