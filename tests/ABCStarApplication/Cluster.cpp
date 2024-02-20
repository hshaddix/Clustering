#include <iostream>
#include <vector>
#include <bitset>
#include <algorithm>
#include <string>

const int STRIP_SIZE = 126; // The total number of positions on a strip.
const int STRIP_NUMBER_BITS = 11; // The number of bits used to represent the strip number.
const int HIT_START_SHIFT = 5; // The shift needed to align the start position bits.
const int POSITION_BITS = 8; // The number of bits representing the hit start position.
const int SIZE_BITMASK_BITS = 13; // The shift to align the size bitmask bits.

struct Hit {
    int stripNumber; // The strip on which the hit is located.
    int position; // The specific position of the hit on the strip.
};

// Comparison operator for sorting hits
bool operator<(const Hit& a, const Hit& b) {
    if (a.stripNumber == b.stripNumber) {
        return a.position < b.position;
    }
    return a.stripNumber < b.stripNumber;
}

// Decode the size bitmask to determine the positions of additional hits
std::vector<Hit> decodeSize(int bitmask, int seedPosition, int stripNumber) {
    std::vector<Hit> hits;
    hits.push_back({stripNumber, seedPosition}); // Include the seed hit in the hits list.
    if (bitmask & 0b001) hits.push_back({stripNumber, seedPosition + 3});
    if (bitmask & 0b010) hits.push_back({stripNumber, seedPosition + 2});
    if (bitmask & 0b100) hits.push_back({stripNumber, seedPosition + 1});
    return hits;
}

// Parse each binary string input into hit objects
std::vector<Hit> parseCluster(const std::string& binary) {
    std::bitset<16> bits(binary);
    int stripNumber = (bits >> STRIP_NUMBER_BITS).to_ulong();
    int seedPosition = ((bits << HIT_START_SHIFT) >> POSITION_BITS).to_ulong();
    int sizeBitmask = static_cast<int>(((bits << SIZE_BITMASK_BITS) >> SIZE_BITMASK_BITS).to_ulong());

    return decodeSize(sizeBitmask, seedPosition, stripNumber);
}

// Merge adjacent hits into clusters
std::vector<std::vector<Hit>> mergeClusters(std::vector<Hit>& hits) {
    if (hits.empty()) return {};

    std::sort(hits.begin(), hits.end()); // Sort hits for merging.

    std::vector<std::vector<Hit>> clusters;
    std::vector<Hit> currentCluster = {hits.front()};

    for (size_t i = 1; i < hits.size(); ++i) {
        // Check for adjacency, including across strip boundaries
        if ((hits[i].stripNumber == currentCluster.back().stripNumber && hits[i].position == currentCluster.back().position + 1) ||
            (hits[i].stripNumber == currentCluster.back().stripNumber + 1 && currentCluster.back().position == STRIP_SIZE - 1 && hits[i].position == 0)) {
            currentCluster.push_back(hits[i]);
        } else {
            clusters.push_back(currentCluster);
            currentCluster = {hits[i]};
        }
    }
    clusters.push_back(currentCluster); // Include the final cluster

    return clusters;
}

// Print the final merged clusters
void printClusters(const std::vector<std::vector<Hit>>& clusters) {
    std::cout << "Final Merged Clusters:" << std::endl;
    for (const auto& cluster : clusters) {
        std::bitset<4> binaryStrip(cluster.front().stripNumber);
        std::bitset<8> binaryStart(cluster.front().position);
        std::bitset<3> binarySize(cluster.size() - 1); // Calculate size for output
        std::cout << "0" + binaryStrip.to_string() + binaryStart.to_string() + binarySize.to_string() << std::endl;
    }
}

int main() {
    std::string binary;
    std::vector<Hit> allHits;

    // Read and process each line of input
    while (std::cin >> binary) {
        auto newHits = parseCluster(binary);
        allHits.insert(allHits.end(), newHits.begin(), newHits.end());
    }

    auto mergedClusters = mergeClusters(allHits);
    printClusters(mergedClusters);

    return 0;
}
