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
            hits.push_back({stripNumber, startPosition}); // Include seed hit for consistency
            break;
        case 0b001: // New cluster starts at initial + 3.
        case 0b010: // A single hit at initial + 2.
        case 0b011: // Hit at initial + 2 and another hit following it.
        case 0b100: // A hit immediately following the initial.
        case 0b101: // A hit, a gap, then another hit at initial + 3.
        case 0b110: // Two hits immediately following the initial.
        case 0b111: // Three following hits.
            hits.push_back({stripNumber, startPosition}); // Include seed hit
            if (bitmask & 0b001) hits.push_back({stripNumber, startPosition + 1});
            if (bitmask & 0b010) hits.push_back({stripNumber, startPosition + 2});
            if (bitmask & 0b100) hits.push_back({stripNumber, startPosition + 3});
            break;
        default:
            std::cerr << "Unexpected bitmask value" << std::endl;
    }
    return hits;
}

std::vector<Hit> parseCluster(const std::string& binary) {
    std::bitset<16> bits(binary);
    int stripNumber = (bits >> 11).to_ulong();
    int startPosition = ((bits << 5) >> 8).to_ulong();
    int sizeBitmask = static_cast<int>(((bits << 13) >> 13).to_ulong());
    
    return decodeSize(sizeBitmask, startPosition, stripNumber);
}

void printClusters(const std::vector<std::vector<Hit>>& clusters) {
    std::cout << "Final Merged Clusters:" << std::endl;
    for (const auto& cluster : clusters) {
        if (cluster.empty()) continue;
        std::bitset<4> binaryStrip(cluster.front().stripNumber);
        std::bitset<8> binaryStart(cluster.front().position);
        std::bitset<3> binarySize(cluster.size() - 1); // Cluster size as bitmask
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

    // Sort hits by strip number and position
    std::sort(allHits.begin(), allHits.end(), [](const Hit& a, const Hit& b) {
        return a.stripNumber < b.stripNumber || (a.stripNumber == b.stripNumber && a.position < b.position);
    });

    // Merge adjacent hits into clusters
    std::vector<std::vector<Hit>> clusters;
    for (const auto& hit : allHits) {
        if (!clusters.empty() && hit.stripNumber == clusters.back().back().stripNumber && 
            (hit.position == clusters.back().back().position || hit.position == clusters.back().back().position + 1)) {
            clusters.back().push_back(hit);
        } else {
            clusters.push_back({hit});
        }
    }

    printClusters(clusters);

    return 0;
}
