#include <iostream>
#include <vector>
#include <bitset>
#include <algorithm>
#include <string>

const int STRIP_SIZE = 126; // Define the size of each strip

// Structure to represent a cluster
struct Cluster {
    int stripNumber;   // Number of the strip where the cluster is located
    int startPosition; // Starting position of the cluster on the strip
    int size;          // Size of the cluster
    int globalStart;   // Global start position of the cluster
    int globalEnd;     // Global end position of the cluster
};

// Function to parse a binary string into a Cluster structure
Cluster parseCluster(const std::string& binary) {
    std::bitset<16> bits(binary); // Convert the binary string to a bitset
    // Extract the strip number, start position, and size from the bitset
    int stripNumber = (bits >> 11).to_ulong();
    int startPosition = ((bits << 5) >> 8).to_ulong();
    int size = ((bits << 13) >> 13).to_ulong(); 

    // Calculate the global start and end positions
    int globalStart = stripNumber * STRIP_SIZE + startPosition;
    int globalEnd = globalStart + size - 1; 

    // Debug print after parsing
    std::cout << "Parsed Cluster - Strip: " << stripNumber 
              << ", Start: " << startPosition 
              << ", Size: " << size << std::endl;

    return {stripNumber, startPosition, size, globalStart, globalEnd};
};

// Function to check if two clusters are adjacent
bool areAdjacent(const Cluster& a, const Cluster& b) {
    // Check adjacency, considering clusters on different strips
    if (a.stripNumber != b.stripNumber) {
        return (a.globalEnd + 1) % STRIP_SIZE == b.startPosition;
    }
    // Regular case for clusters on the same strip
    return a.globalEnd + 1 == b.globalStart;
}

// Function to merge adjacent clusters into a single cluster
std::vector<Cluster> mergeClusters(std::vector<Cluster>& clusters) {
    if (clusters.empty()) return {}; // Return empty if no clusters

    // Sort clusters based on their global end positions
    std::sort(clusters.begin(), clusters.end(), [](const Cluster& a, const Cluster& b) {
        return a.globalEnd < b.globalEnd;
    });

    std::vector<Cluster> merged; // Vector to store merged clusters
    Cluster current = clusters[0]; // Start with the first cluster

    // Iterate through the clusters and merge adjacent ones
    for (size_t i = 1; i < clusters.size(); ++i) {
        if (areAdjacent(current, clusters[i])) {
            // Merge clusters by increasing the size
            current.size += clusters[i].size;
            current.globalEnd = current.globalStart + current.size - 1;

            // Debug print after checking adjacency
            std::cout << "Merged Cluster - Strip: " << current.stripNumber 
                      << ", Start: " << current.startPosition 
                      << ", Size: " << current.size << std::endl;
        } else {
            merged.push_back(current); // Add the current cluster to the merged list
            current = clusters[i]; // Move to the next cluster
        }
    }

    // Add the last cluster to the merged list
    merged.push_back(current);
    return merged;
}

// Function to convert a Cluster structure back to a binary string
std::string toBinaryString(const Cluster& cluster) {
    // Convert each part of the cluster to binary
    std::bitset<4> binaryStrip(cluster.stripNumber);
    std::bitset<8> binaryStart(cluster.startPosition);
    std::bitset<3> binarySize(cluster.size);

    // Combine the binary parts into a single string
    return "0" + binaryStrip.to_string() + binaryStart.to_string() + binarySize.to_string();
}

int main() {
    std::string binary; // String to store each line of binary input
    std::vector<Cluster> clusters; // Vector to store the parsed clusters

    // Read binary input and parse each line into a Cluster
    while (std::cin >> binary) {
        clusters.push_back(parseCluster(binary));
    }

    // Merge adjacent clusters
    auto mergedClusters = mergeClusters(clusters);

    // Output the merged clusters in binary format
    for (const auto& cluster : mergedClusters) {
        std::cout << toBinaryString(cluster) << std::endl;
    }

    return 0;
}
