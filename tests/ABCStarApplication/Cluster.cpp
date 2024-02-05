#include <iostream>
#include <vector>
#include <bitset>
#include <algorithm>
#include <string>

const int STRIP_SIZE = 126; // Define the size of each strip

struct Cluster {
    int stripNumber;   // Number of the strip where the cluster is located
    int startPosition; // Starting position of the cluster on the strip
    int size;          // Size of the cluster
    int globalStart;   // Global start position of the cluster
    int globalEnd;     // Global end position of the cluster
};

Cluster parseCluster(const std::string& binary) {
    std::bitset<16> bits(binary);
    int stripNumber = (bits >> 11).to_ulong();
    int startPosition = ((bits << 5) >> 8).to_ulong();
    int size = ((bits << 13) >> 13).to_ulong(); 
    int globalStart = stripNumber * STRIP_SIZE + startPosition;
    int globalEnd = globalStart + size - 1; 

    // Print after parsing
    std::cout << "Parsed Cluster - Strip: " << stripNumber 
              << ", Start: " << startPosition 
              << ", Size: " << size << std::endl;

    return {stripNumber, startPosition, size, globalStart, globalEnd};
};

bool areAdjacent(const Cluster& a, const Cluster& b) {
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

    // Print before outputting final merged clusters
    std::cout << "Final Merged Clusters:" << std::endl;
    auto mergedClusters = mergeClusters(clusters);
    for (const auto& cluster : mergedClusters) {
        // Print the final merged cluster details
        std::cout << "Final Cluster - Strip: " << cluster.stripNumber 
                  << ", Start: " << cluster.startPosition 
                  << ", Size: " << cluster.size << std::endl;
        std::cout << toBinaryString(cluster) << std::endl;
    }

    return 0;
}
