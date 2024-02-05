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
    
    // Correctly convert bit manipulation result to int before passing to decodeSize
    int sizeBitmask = static_cast<int>((bits << 13) >> 13).to_ulong();
    int size = decodeSize(sizeBitmask);

    int globalStart = stripNumber * STRIP_SIZE + startPosition;
    int globalEnd = globalStart + size - 1;

    std::cout << "Parsed Cluster - Strip: " << stripNumber 
              << ", Start: " << startPosition 
              << ", Size: " << size << std::endl;

    return {stripNumber, startPosition, size, globalStart, globalEnd};
};

int main() {
    std::string binary;
    std::vector<Cluster> clusters;

    while (std::cin >> binary) {
        clusters.push_back(parseCluster(binary));
    }

    auto mergedClusters = mergeClusters(clusters);

    std::cout << "Final Merged Clusters:" << std::endl;
    for (const auto& cluster : mergedClusters) {
        std::cout << "Final Cluster - Strip: " << cluster.stripNumber 
                  << ", Start: " << cluster.startPosition 
                  << ", Size: " << cluster.size << std::endl;
        std::cout << toBinaryString(cluster) << std::endl;
    }

    return 0;
}
