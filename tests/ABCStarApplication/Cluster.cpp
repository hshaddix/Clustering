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

// Function to decode the bitmask and determine hit positions relative to the seed hit
std::vector<Hit> decodeSize(int bitmask, int seedPosition, int stripNumber) {
    std::vector<Hit> hits;
    // Always include the seed hit as the first hit
    hits.push_back({stripNumber, seedPosition});

    // Decode the bitmask to determine additional hits
    if (bitmask & 0b001) hits.push_back({stripNumber, seedPosition + 1});
    if (bitmask & 0b010) hits.push_back({stripNumber, seedPosition + 2});
    if (bitmask & 0b100) hits.push_back({stripNumber, seedPosition + 3});

    return hits;
}

std::vector<Hit> parseCluster(const std::string& binary) {
    std::bitset<16> bits(binary);
    int stripNumber = (bits >> 11).to_ulong();
    int seedPosition = ((bits << 5) >> 8).to_ulong();
    int sizeBitmask = static_cast<int>(((bits << 13) >> 13).to_ulong());

    auto hits = decodeSize(sizeBitmask, seedPosition, stripNumber);

    std::cout << "Parsed Cluster - Strip: " << stripNumber 
              << ", Seed Hit: " << seedPosition 
              << ", Size Bitmask: " << std::bitset<3>(sizeBitmask) << std::endl;

    return hits;
}

// Function to merge hits into clusters based on adjacency
std::vector<std::vector<Hit>> mergeClusters(const std::vector<Hit>& hits) {
    std::vector<std::vector<Hit>> clusters;
    for (const auto& hit : hits) {
        // Check if the current hit can be merged into the last cluster
        bool merged = false;
        for (auto& cluster : clusters) {
            if (hit.stripNumber == cluster.back().stripNumber && hit.position == cluster.back().position + 1) {
                cluster.push_back(hit);
                merged = true;
                break;
            }
        }
        // If not merged, start a new cluster
        if (!merged) {
            clusters.push_back({hit});
        }
    }

    return clusters;
}

void printClusters(const std::vector<std::vector<Hit>>& clusters) {
    std::cout << "Final Merged Clusters:" << std::endl;
    for (const auto& cluster : clusters) {
        if (cluster.empty()) continue;
        std::bitset<4> binaryStrip(cluster.front().stripNumber);
        int start = cluster.front().position;
        int size = cluster.size(); // Number of hits in the cluster
        std::bitset<3> binarySize(size - 1); // Convert size to a 3-bit representation
        std::cout << "0" + binaryStrip.to_string() + std::bitset<8>(start).to_string() + binarySize.to_string() << std::endl;
    }
}

int main() {
    std::string binary;
    std::vector<Hit> allHits;

    while (std::cin >> binary) {
        auto newHits = parseCluster(binary);
        allHits.insert(allHits.end(), newHits.begin(), newHits.end());
    }

    auto mergedClusters = mergeClusters(allHits);
    printClusters(mergedClusters);

    return 0;
}
