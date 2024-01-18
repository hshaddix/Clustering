#include <iostream>
#include <vector>
#include <sstream>
#include <iomanip>

struct Cluster {
    int stripNumber;
    int startPosition;
    int size;

    // Constructor for easy initialization
    Cluster(int strip, int start, int sz) : stripNumber(strip), startPosition(start), size(sz) {}

    // Helper function to calculate the global position
    int globalPosition() const {
        return stripNumber * 126 + startPosition;
    }

    // Convert cluster data to 16-bit format
    unsigned int to16BitFormat() const {
        return (stripNumber << 11) | (startPosition << 3) | size;
    }
};

// Function to parse input string into Cluster objects
std::vector<Cluster> parseInput(const std::string& input) {
    std::vector<Cluster> clusters;
    std::istringstream iss(input);
    unsigned int value;

    while (iss >> std::hex >> value) {
        int strip = (value >> 11) & 0xF;
        int start = (value >> 3) & 0xFF;
        int size = value & 0x7;
        clusters.emplace_back(strip, start, size);
    }

    return clusters;
}

// Function to merge clusters if adjacent
std::vector<Cluster> mergeClusters(const std::vector<Cluster>& clusters) {
    std::vector<Cluster> merged;
    if (clusters.empty()) return merged;

    Cluster current = clusters[0];
    for (size_t i = 1; i < clusters.size(); ++i) {
        if (clusters[i].globalPosition() == current.globalPosition() + current.size) {
            current.size += clusters[i].size; // Extend the current cluster
        } else {
            merged.push_back(current); // Save the current cluster
            current = clusters[i];
        }
    }
    merged.push_back(current); // Add the last cluster

    return merged;
}

// Main function
int main() {
    std::string input;
    std::getline(std::cin, input);

    std::vector<Cluster> clusters = parseInput(input);
    std::vector<Cluster> mergedClusters = mergeClusters(clusters);

    for (const auto& cluster : mergedClusters) {
        std::cout << std::hex << std::setw(4) << std::setfill('0') << cluster.to16BitFormat() << " ";
    }
    std::cout << std::endl;

    return 0;
}
