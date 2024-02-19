#include <iostream>
#include <vector>
#include <bitset>
#include <algorithm>
#include <string>

const int STRIP_SIZE = 126; // Define constant for strip size, though it's not used in this snippet.

struct Hit {
    int stripNumber; // Identifies which strip the hit is on.
    int position;    // The position of the hit on the strip.
};

// Overload the '<' operator to compare hits, facilitating sorting.
bool operator<(const Hit& a, const Hit& b) {
    if (a.stripNumber == b.stripNumber) {
        return a.position < b.position; // Sort by position if on the same strip.
    }
    return a.stripNumber < b.stripNumber; // Otherwise, sort by strip number.
}

// Decode the size bitmask to determine additional hits relative to the seed position.
std::vector<Hit> decodeSize(int bitmask, int seedPosition, int stripNumber) {
    std::vector<Hit> hits;
    hits.push_back({stripNumber, seedPosition}); // Always include the seed hit.
    // Check each bit in the bitmask and add hits accordingly.
    if (bitmask & 0b001) hits.push_back({stripNumber, seedPosition + 3});
    if (bitmask & 0b010) hits.push_back({stripNumber, seedPosition + 2});
    if (bitmask & 0b100) hits.push_back({stripNumber, seedPosition + 1});

    // Debug: Print each decoded hit.
    for (const auto& hit : hits) {
        std::cout << "Decoded Hit - Strip: " << hit.stripNumber << ", Position: " << hit.position << std::endl;
    }

    return hits;
}

// Parse the binary input string into hits based on the specified bitmask.
std::vector<Hit> parseCluster(const std::string& binary) {
    std::bitset<16> bits(binary); // Convert binary string to bitset for easy manipulation.
    int stripNumber = (bits >> 11).to_ulong(); // Extract strip number.
    int seedPosition = ((bits << 5) >> 8).to_ulong(); // Extract seed position.
    int sizeBitmask = static_cast<int>(((bits << 13) >> 13).to_ulong()); // Extract size bitmask.

    auto hits = decodeSize(sizeBitmask, seedPosition, stripNumber); // Decode additional hits based on bitmask.

    // Debug: Print summary of parsed cluster.
    std::cout << "Parsed Cluster - Strip: " << stripNumber 
              << ", Seed Hit: " << seedPosition 
              << ", Size Bitmask: " << std::bitset<3>(sizeBitmask) << std::endl;

    return hits;
}

// Merge adjacent hits into clusters.
std::vector<std::vector<Hit>> mergeClusters(std::vector<Hit>& hits) {
    if (hits.empty()) return {}; // Return early if no hits.

    // Sort hits to ensure adjacency can be accurately determined.
    std::sort(hits.begin(), hits.end());

    std::vector<std::vector<Hit>> clusters; // To hold merged clusters.
    std::vector<Hit> currentCluster = {hits.front()}; // Start the first cluster.

    for (size_t i = 1; i < hits.size(); ++i) {
        // Check if current hit is adjacent to the last hit in the current cluster.
        if (hits[i].stripNumber == currentCluster.back().stripNumber && hits[i].position == currentCluster.back().position + 1) {
            currentCluster.push_back(hits[i]); // Add to current cluster.
        } else {
            clusters.push_back(currentCluster); // Start a new cluster.
            currentCluster = {hits[i]};
        }
    }

    clusters.push_back(currentCluster); // Add the last cluster.

    return clusters; // Return the merged clusters.
}

// Print the final merged clusters.
void printClusters(const std::vector<std::vector<Hit>>& clusters) {
    std::cout << "Final Merged Clusters:" << std::endl;
    for (const auto& cluster : clusters) {
        std::bitset<4> binaryStrip(cluster.front().stripNumber); // Convert strip number to binary.
        std::bitset<8> binaryStart(cluster.front().position); // Convert start position to binary.
        std::bitset<3> binarySize(cluster.size() - 1); // Convert size to binary, adjusting for 0-based indexing.
        std::cout << "0" + binaryStrip.to_string() + binaryStart.to_string() + binarySize.to_string() << std::endl;
    }
}

int main() {
    std::string binary;
    std::vector<Hit> allHits;

    // Read binary strings from input until EOF.
    while (std::cin >> binary) {
        auto newHits = parseCluster(binary); // Parse each binary string into hits.
        allHits.insert(allHits.end(), newHits.begin(), newHits.end()); // Collect all hits.
    }

    auto mergedClusters = mergeClusters(allHits); // Merge hits into clusters.
    printClusters(mergedClusters); // Print the final merged clusters.

    return 0;
}
