// Code Written by Hayden Shaddix for FPGA EFTracking project involving clustering 

#include <iostream>
#include <vector>
#include <bitset>
#include <algorithm>
#include <string>

const int STRIP_SIZE = 126;  // Constant representing the size of each strip

// Structure to represent a cluster
struct Cluster {
    int stripNumber;      // The number of the strip on which the cluster is located
    int startPosition;    // The starting position of the cluster on the strip
    int size;             // The size of the cluster
    int globalStart;      // The global start position of the cluster
    int globalEnd;        // The global end position of the cluster
};

// Function to parse a binary string into a Cluster structure
Cluster parseCluster(const std::string& binary) {
    std::bitset<16> bits(binary);  // Convert binary string to a bitset for easy manipulation
    int stripNumber = (bits >> 11).to_ulong();  // Extract the strip number (first 4 bits)
    int startPosition = ((bits << 5) >> 8).to_ulong();  // Extract the start position (next 8 bits)
    int size = ((bits << 13) >> 13).to_ulong() + 1;  // Extract size (last 3 bits) and add 1 (as per program's logic)

    int globalStart = stripNumber * STRIP_SIZE + startPosition;  // Calculate the global start position
    int globalEnd = globalStart + size - 1;  // Calculate the global end position

    return {stripNumber, startPosition, size, globalStart, globalEnd};  // Return the constructed Cluster
};

// Function to check if two clusters are adjacent
bool areAdjacent(const Cluster& a, const Cluster& b) {
    if (a.stripNumber != b.stripNumber) {
        // Special case for clusters on different strips
        return (a.globalEnd + 1) % STRIP_SIZE == b.startPosition;
    }
    return a.globalEnd + 1 == b.globalStart;  // Regular case for clusters on the same strip
}

// Function to merge adjacent clusters into a single cluster
std::vector<Cluster> mergeClusters(std::vector<Cluster>& clusters) {
    if (clusters.empty()) return {};  // Return an empty vector if there are no clusters

    // Sort clusters based on their global end positions
    std::sort(clusters.begin(), clusters.end(), [](const Cluster& a, const Cluster& b) {
        return a.globalEnd < b.globalEnd;
    });

    std::vector<Cluster> merged;  // Vector to store merged clusters
    Cluster current = clusters[0];  // Start with the first cluster

    // Iterate through the clusters and merge adjacent clusters
    for (size_t i = 1; i < clusters.size(); ++i) {
        if (areAdjacent(current, clusters[i])) {
            // Merge clusters by adding sizes
            current.size += clusters[i].size;
            current.globalEnd = current.globalStart + current.size - 1;
        } else {
            merged.push_back(current);  // Add the current cluster to the merged list
            current = clusters[i];      // Move to the next cluster
        }
    }

    merged.push_back(current);  // Add the last cluster to the merged list
    return merged;  // Return the merged clusters
}

// Function to convert a Cluster structure back to a binary string
std::string toBinaryString(const Cluster& cluster) {
    std::bitset<4> binaryStrip(cluster.stripNumber);  // Convert strip number to binary
    std::bitset<8> binaryStart(cluster.startPosition);  // Convert start position to binary
    std::bitset<3> binarySize(cluster.size - 1);  // Convert size to binary (size - 1 as per logic)

    // Concatenate the binary parts into a single string
    return "0" + binaryStrip.to_string() + binaryStart.to_string() + binarySize.to_string();
}

int main() {
    std::string binary;  // String to store each line of binary input
    std::vector<Cluster> clusters;  // Vector to store the parsed clusters

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
