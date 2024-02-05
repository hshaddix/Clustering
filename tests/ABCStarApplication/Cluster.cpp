#include <iostream>
#include <vector>
#include <bitset>
#include <algorithm>
#include <string>

const int STRIP_SIZE = 126; // Define the size of each strip

struct Cluster {
    int stripNumber;   // Number of the strip where the cluster is located
    int startPosition; // Starting position of the cluster on the strip
    int size;          // Total size of the cluster, including the seed hit
    int globalStart;   // Global start position of the cluster
    int globalEnd;     // Global end position of the cluster
};

// Function to decode the size bitmask into the total number of hits
int decodeSize(int bitmask) {
    // Adds 1 to the bitmask value to account for the seed hit
    return bitmask + 1;
}

Cluster parseCluster(const std::string& binary) {
    std::bitset<16> bits(binary);
    int stripNumber = (bits >> 11).to_ulong();
    int startPosition = ((bits << 5) >> 8).to_ulong();
    // Decode the size from the last 3 bits
    int size = decodeSize((bits << 13) >> 13).to_ulong();

    int globalStart = stripNumber * STRIP_SIZE + startPosition;
    // The global end is calculated based on the total size (including the seed hit)
    int globalEnd = globalStart + size - 1;

    // Print details after parsing each cluster
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

    // Sort clusters based on their global start positions
    std::sort(clusters.begin(), clusters.end(), [](const Cluster& a, const Cluster& b) {
        return a.globalStart < b.globalStart;
    });

    std::vector<Cluster> merged;
    Cluster current = clusters[0];

    for (size_t i = 1; i < clusters.size(); ++i) {
        if (areAdjacent(current, clusters[i])) {
            // Merge clusters by adding sizes and adjusting the global end
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
    std::bitset<3> binarySize(cluster.size - 1); // Convert total size back to bitmask

    return "0" + binaryStrip.to_string() + binaryStart.to_string() + binarySize.to_string();
}

int main() {
    std::string binary;
    std::vector<Cluster> clusters;

    while (std::cin >> binary) {
        clusters.push_back(parseCluster(binary));
    }

    auto mergedClusters = mergeClusters(clusters);

    // Print details before outputting final merged clusters
    std::cout << "Final Merged Clusters:" << std::endl;
    for (const auto& cluster : mergedClusters) {
        std::cout << "Final Cluster - Strip: " << cluster.stripNumber 
                  << ", Start: " << cluster.startPosition 
                  << ", Size: " << cluster.size << std::endl;
        std::cout << toBinaryString(cluster) << std::endl;
    }

    return 0;
}
