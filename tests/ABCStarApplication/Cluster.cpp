#include <iostream>
#include <vector>
#include <bitset>
#include <algorithm>
#include <string>

const int STRIP_SIZE = 126; // Assuming each strip has positions 0 to 125.

struct Hit {
    int stripNumber;
    int position;
};

bool operator<(const Hit& a, const Hit& b) {
    if (a.stripNumber == b.stripNumber) {
        return a.position < b.position;
    }
    return a.stripNumber < b.stripNumber;
}

bool areAdjacent(const Hit& a, const Hit& b) {
    // Check if hits are on the same strip and adjacent.
    if (a.stripNumber == b.stripNumber && a.position + 1 == b.position) {
        return true;
    }
    // Check for adjacency at strip boundary.
    if (a.stripNumber + 1 == b.stripNumber && a.position == STRIP_SIZE - 1 && b.position == 0) {
        return true;
    }
    return false;
}

std::vector<Hit> decodeSize(int bitmask, int seedPosition, int stripNumber) {
    std::vector<Hit> hits;
    hits.push_back({stripNumber, seedPosition}); // Always include the seed hit.
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

    std::cout << "Parsed Cluster - Strip: " << stripNumber 
              << ", Seed Hit: " << seedPosition 
              << ", Size Bitmask: " << std::bitset<3>(sizeBitmask) << std::endl;

    return decodeSize(sizeBitmask, seedPosition, stripNumber);
}

std::vector<std::vector<Hit>> mergeClusters(std::vector<Hit>& hits) {
    if (hits.empty()) return {};

    std::sort(hits.begin(), hits.end()); // Sort hits for proper merging.

    std::vector<std::vector<Hit>> clusters;
    std::vector<Hit> currentCluster = {hits.front()};

    for (size_t i = 1; i < hits.size(); ++i) {
        if (areAdjacent(currentCluster.back(), hits[i])) {
            currentCluster.push_back(hits[i]);
        } else {
            clusters.push_back(currentCluster);
            currentCluster = {hits[i]};
        }
    }
    clusters.push_back(currentCluster); // Include the last cluster.

    return clusters;
}

void printClusters(const std::vector<std::vector<Hit>>& clusters) {
    std::cout << "Final Merged Clusters:" << std::endl;
    for (const auto& cluster : clusters) {
        std::bitset<4> binaryStrip(cluster.front().stripNumber);
        std::bitset<8> binaryStart(cluster.front().position);
        std::bitset<3> binarySize(cluster.size() - 1); // Adjust size for 0-based index.
        std::cout << "0" + binaryStrip.to_string() + binaryStart.to_string() + binarySize.to_string() << std::endl;
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
