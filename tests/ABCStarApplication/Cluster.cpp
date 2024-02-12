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

std::vector<Hit> decodeSize(int bitmask, int startPosition, int stripNumber) {
    std::vector<Hit> hits;
    switch (bitmask) {
        case 0b000: // No additional hits.
            // No hits are added for 000 as it indicates no additional hits beyond the initial one
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
        case 0b100: // A hit following the initial.
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
    }
    return hits;
}

std::vector<Hit> parseCluster(const std::string& binary) {
    std::bitset<16> bits(binary);
    int stripNumber = (bits >> 11).to_ulong();
    int startPosition = ((bits << 5) >> 8).to_ulong();
    int sizeBitmask = static_cast<int>(((bits << 13) >> 13).to_ulong());

    std::cout << "Parsed Cluster - Strip: " << stripNumber 
              << ", Start: " << startPosition 
              << ", Size Bitmask: " << std::bitset<3>(sizeBitmask) << std::endl;

    // Including the start position as a hit for all cases except 000
    std::vector<Hit> hits = {{stripNumber, startPosition}};
    std::vector<Hit> additionalHits = decodeSize(sizeBitmask, startPosition, stripNumber);
    hits.insert(hits.end(), additionalHits.begin(), additionalHits.end());
    return hits;
}

std::vector<std::vector<Hit>> mergeClusters(std::vector<Hit>& hits) {
    // Sort hits by strip number and position for accurate merging
    std::sort(hits.begin(), hits.end(), [](const Hit& a, const Hit& b) {
        return a.stripNumber < b.stripNumber || (a.stripNumber == b.stripNumber && a.position < b.position);
    });

    std::vector<std::vector<Hit>> clusters;
    for (const auto& hit : hits) {
        if (clusters.empty() || clusters.back().back().stripNumber != hit.stripNumber ||
            clusters.back().back().position + 1 < hit.position) {
            clusters.push_back({hit}); // Start a new cluster for non-adjacent hits
        } else {
            clusters.back().push_back(hit); // Add to the existing cluster for adjacent hits
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
        int size = cluster.size();
        std::bitset<3> binarySize(size - 1); // Using size - 1 to fit the 3-bit size representation
        std::cout << "0" + binaryStrip.to_string() + binaryStart.to_string() + binarySize.to_string() << std::endl;
    }
}

int main() {
    std::string binary;
    std::vector<Hit> allHits;

    while (std::cin >> binary) {
        auto hits = parseCluster(binary);
        allHits.insert(allHits.end(), hits.begin(), hits.end());
    }

    auto mergedClusters = mergeClusters(allHits);
    printClusters(mergedClusters);

    return 0;
}
