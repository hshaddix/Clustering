#include <iostream>
#include <vector>
#include <bitset>
#include <algorithm>
#include <string>

const int STRIP_SIZE = 126; // Number of Channels in ABC*

struct Cluster {
    int stripNumber;   // ABCstar ID/Number
    int startPosition; // Starting position of the cluster on the strip
    std::vector<int> hits; // Positions of hits relative to start
};

std::vector<int> decodeSize(int bitmask, int startPosition) {
    std::vector<int> positions;
    // Handle each bitmask case as specified
    switch (bitmask) {
        case 0b000: // No additional hits.
            break;
        case 0b001: // New cluster starts at initial + 3.
            positions.push_back(startPosition + 3);
            break;
        case 0b010: // A single hit at initial + 2.
            positions.push_back(startPosition + 2);
            break;
        case 0b100: // A hit following the initial.
            positions.push_back(startPosition + 1);
            break;
        case 0b110: // Two following hits.
            positions.push_back(startPosition + 1);
            positions.push_back(startPosition + 2);
            break;
        case 0b011: // Hit at initial + 2 and another hit following it.
            positions.push_back(startPosition + 2);
            positions.push_back(startPosition + 3);
            break;
        case 0b101: // A hit, a gap, then another hit at initial + 3.
            positions.push_back(startPosition + 1);
            positions.push_back(startPosition + 3);
            break;
        case 0b111: // Three following hits.
            positions.push_back(startPosition + 1);
            positions.push_back(startPosition + 2);
            positions.push_back(startPosition + 3);
            break;
        default:
            std::cerr << "Unexpected bitmask value" << std::endl;
    }
    return positions;
}

Cluster parseCluster(const std::string& binary) {
    std::bitset<16> bits(binary);
    int stripNumber = (bits >> 11).to_ulong();
    int startPosition = ((bits << 5) >> 8).to_ulong();
    int sizeBitmask = static_cast<int>(((bits << 13) >> 13).to_ulong());
    
    auto hits = decodeSize(sizeBitmask, startPosition);

    std::cout << "Parsed Cluster - Strip: " << stripNumber 
              << ", Start: " << startPosition 
              << ", Size Bitmask: " << std::bitset<3>(sizeBitmask) << std::endl;

    return {stripNumber, startPosition, hits};
}

// Adjusted to work with the new structure of Cluster
bool areAdjacent(const Cluster& a, const Cluster& b) {
    for (int hit : a.hits) {
        for (int nextHit : b.hits) {
            if (hit + 1 == nextHit) return true;
        }
    }
    return false;
}

std::vector<Cluster> mergeClusters(std::vector<Cluster>& clusters) {
    if (clusters.empty()) return {};

    std::sort(clusters.begin(), clusters.end(), [](const Cluster& a, const Cluster& b) {
        return a.startPosition < b.startPosition;
    });

    std::vector<Cluster> merged;
    Cluster current = clusters.front();

    for (size_t i = 1; i < clusters.size(); ++i) {
        if (areAdjacent(current, clusters[i])) {
            current.hits.insert(current.hits.end(), clusters[i].hits.begin(), clusters[i].hits.end());
            std::sort(current.hits.begin(), current.hits.end());
            current.hits.erase(unique(current.hits.begin(), current.hits.end()), current.hits.end());
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
    // Simplified output assuming size not directly relevant for final output
    std::bitset<3> binarySize(0); // Placeholder for output

    return "0" + binaryStrip.to_string() + binaryStart.to_string() + binarySize.to_string();
}

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
                  << ", Start: " << cluster.startPosition << ", Hits: ";
        for (auto hit : cluster.hits) {
            std::cout << hit << " ";
        }
        std::cout << std::endl;
    }

    return 0;
}
