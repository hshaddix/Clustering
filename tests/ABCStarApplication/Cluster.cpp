#include <iostream>
#include <vector>
#include <bitset>
#include <algorithm>
#include <string>

const int STRIP_SIZE = 126; // Number of Channels in ABC*

struct Hit {
    int stripNumber;   // ABCstar ID/Number
    int position;      // Position of the hit on the strip
};

// Decode the bitmask into a vector of hit positions relative to the start position and strip number
std::vector<Hit> decodeSize(int bitmask, int startPosition, int stripNumber) {
    std::vector<Hit> hits;
    // Interpret bitmask according to specified rules
    switch (bitmask) {
        case 0b000: // No additional hits.
            break;
        case 0b001: // New cluster starts at initial + 3.
            hits.push_back({stripNumber, startPosition + 3});
            break;
        case 0b010: // A single hit at initial + 2.
            hits.push_back({stripNumber, startPosition + 2});
            break;
        case 0b011: // Hit at initial + 2 and another hit following it.
            hits.push_back({stripNumber, startPosition + 2});
            hits.push_back({stripNumber, startPosition + 3});
            break;
        case 0b100: // A hit immediately following the initial.
            hits.push_back({stripNumber, startPosition + 1});
            break;
        case 0b101: // A hit, a gap, then another hit at initial + 3.
            hits.push_back({stripNumber, startPosition + 1});
            hits.push_back({stripNumber, startPosition + 3});
            break;
        case 0b110: // Two hits immediately following the initial.
            hits.push_back({stripNumber, startPosition + 1});
            hits.push_back({stripNumber, startPosition + 2});
            break;
        case 0b111: // Three following hits.
            hits.push_back({stripNumber, startPosition + 1});
            hits.push_back({stripNumber, startPosition + 2});
            hits.push_back({stripNumber, startPosition + 3});
            break;
        default:
            std::cerr << "Unexpected bitmask value" << std::endl;
    }
    return hits;
}

// Parses a binary string into a vector of Hit structures.
std::vector<Hit> parseCluster(const std::string& binary) {
    std::bitset<16> bits(binary);
    int stripNumber = (bits >> 11).to_ulong();
    int startPosition = ((bits << 5) >> 8).to_ulong();
    int sizeBitmask = static_cast<int>(((bits << 13) >> 13).to_ulong());
    
    auto hits = decodeSize(sizeBitmask, startPosition, stripNumber);

    std::cout << "Parsed Cluster - Strip: " << stripNumber 
              << ", Start: " << startPosition 
              << ", Size Bitmask: " << std::bitset<3>(sizeBitmask) << std::endl;

    return hits;
}

// Merges adjacent hits into clusters and prints the final clusters.
void mergeAndPrintClusters(std::vector<Hit>& hits) {
    if (hits.empty()) return;

    // Sort hits by strip number first, then position
    std::sort(hits.begin(), hits.end(), [](const Hit& a, const Hit& b) {
        return a.stripNumber < b.stripNumber || (a.stripNumber == b.stripNumber && a.position < b.position);
    });

    // Merge adjacent hits into clusters
    std::vector<std::vector<Hit>> clusters;
    std::vector<Hit> currentCluster;
    currentCluster.push_back(hits[0]);

    for (size_t i = 1; i < hits.size(); ++i) {
        if (hits[i].stripNumber == currentCluster.back().stripNumber && hits[i].position == currentCluster.back().position + 1) {
            // If the current hit is adjacent to the last hit in the current cluster, add it to the cluster
            currentCluster.push_back(hits[i]);
        } else {
            // Otherwise, start a new cluster
            clusters.push_back(currentCluster);
            currentCluster.clear();
            currentCluster.push_back(hits[i]);
        }
    }
    clusters.push_back(currentCluster); // Add the last cluster

    // Print clusters
    std::cout << "Final Merged Clusters:" << std::endl;
    for (const auto& cluster : clusters) {
        if (!cluster.empty()) {
            std::bitset<4> binaryStrip(cluster[0].stripNumber);
            std::bitset<8> binaryStart(cluster[0].position);
            int size = cluster.size(); // Cluster size is the number of hits in the cluster
            std::bitset<3> binarySize(size - 1); // Convert size to 3-bit bitmask
            
            std::cout << "0" + binaryStrip.to_string() + binaryStart.to_string() + binarySize.to_string() << std::endl;
        }
    }
}

int main() {
    std::string binary;
    std::vector<Hit> allHits;

    while (std::cin >> binary) {
        auto hits = parseCluster(binary);
        allHits.insert(allHits.end(), hits.begin(), hits.end());
    }

    mergeAndPrintClusters(allHits);

    return 0;
}
