#include <iostream>
#include <vector>
#include <bitset>
#include <algorithm>
#include <string>

const int STRIP_SIZE = 126;

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

std::vector<Hit> decodeSize(int bitmask, int seedPosition, int stripNumber) {
    std::vector<Hit> hits;
    hits.push_back({stripNumber, seedPosition}); // Include the seed hit
    if (bitmask & 0b001) hits.push_back({stripNumber, seedPosition + 3});
    if (bitmask & 0b010) hits.push_back({stripNumber, seedPosition + 2});
    if (bitmask & 0b100) hits.push_back({stripNumber, seedPosition + 1});
    return hits;
}

std::vector<Hit> parseCluster(const std::string& binary) {
    std::bitset<16> bits(binary);
    int stripNumber = (bits >> 11).to_ulong();
    int seedPosition = ((bits << 5) >> 8).to_ulong();
    int sizeBitmask = static_cast<int>(((bits << 13) >> 13).to_ulong());

    return decodeSize(sizeBitmask, seedPosition, stripNumber);
}

std::vector<std::vector<Hit>> mergeClusters(std::vector<Hit>& hits) {
    if (hits.empty()) return {};

    // Sort hits by strip number and then by position
    std::sort(hits.begin(), hits.end());

    std::vector<std::vector<Hit>> clusters;
    std::vector<Hit> currentCluster = {hits.front()};

    for (size_t i = 1; i < hits.size(); ++i) {
        if (hits[i].stripNumber == currentCluster.back().stripNumber && hits[i].position == currentCluster.back().position + 1) {
            // If the current hit is adjacent to the last hit in the current cluster
            currentCluster.push_back(hits[i]);
        } else {
            // If not adjacent, start a new cluster
            clusters.push_back(currentCluster);
            currentCluster = {hits[i]};
        }
    }

    // Add the last cluster
    clusters.push_back(currentCluster);

    return clusters;
}

void printClusters(const std::vector<std::vector<Hit>>& clusters) {
    for (const auto& cluster : clusters) {
        std::bitset<4> binaryStrip(cluster.front().stripNumber);
        std::bitset<8> binaryStart(cluster.front().position);
        std::bitset<3> binarySize(cluster.size() - 1); // Using size - 1 for correct representation
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
