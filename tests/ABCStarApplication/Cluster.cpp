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

std::vector<Hit> decodeSize(int bitmask, int seedPosition, int stripNumber) {
    std::vector<Hit> hits;
    // Include the seed hit
    hits.push_back({stripNumber, seedPosition});
    // Decode the bitmask to determine additional hits
    if (bitmask & 0b001) hits.push_back({stripNumber, seedPosition + 3});
    if (bitmask & 0b010) hits.push_back({stripNumber, seedPosition + 2});
    if (bitmask & 0b100) hits.push_back({stripNumber, seedPosition + 1});

    return hits;
}

std::vector<Hit> parseCluster(const std::string& binary) {
    std::bitset<16> bits(binary);
    int stripNumber = (bits >> 11).to_ulong();
    int startPosition = ((bits << 5) >> 8).to_ulong(); // Note: This converts binary to decimal directly
    int sizeBitmask = static_cast<int>(((bits << 13) >> 13).to_ulong());

    auto hits = decodeSize(sizeBitmask, startPosition, stripNumber);

    // Debugging print
    std::cout << "Debug - Parsed Cluster - Strip: " << stripNumber 
              << ", Start Position: " << startPosition 
              << ", Size Bitmask: " << std::bitset<3>(sizeBitmask) << std::endl;

    return hits;
}

std::vector<std::vector<Hit>> mergeClusters(std::vector<Hit>& hits) {
    std::vector<std::vector<Hit>> clusters;
    for (const auto& hit : hits) {
        // Debugging print for hit position
        std::cout << "Debug - Hit Position: " << hit.position << std::endl;

        if (clusters.empty() || clusters.back().back().stripNumber != hit.stripNumber ||
            clusters.back().back().position + 1 < hit.position) {
            clusters.push_back({hit});
        } else {
            clusters.back().push_back(hit);
        }
    }

    return clusters;
}

void printClusters(const std::vector<std::vector<Hit>>& clusters) {
    std::cout << "Final Merged Clusters:" << std::endl;
    for (const auto& cluster : clusters) {
        std::bitset<4> binaryStrip(cluster.front().stripNumber);
        int start = cluster.front().position;
        std::bitset<8> binaryStart(start);
        std::bitset<3> binarySize(cluster.size() - 1); // Assuming size as number of hits
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
