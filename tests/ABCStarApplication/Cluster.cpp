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

// Custom comparator for sorting hits
struct HitComparator {
    bool operator()(const Hit& a, const Hit& b) const {
        return a.stripNumber < b.stripNumber || (a.stripNumber == b.stripNumber && a.position < b.position);
    }
};

std::vector<Hit> decodeSize(int bitmask, int startPosition, int stripNumber) {
    std::vector<Hit> hits;
    if (bitmask & 0b001) hits.push_back({stripNumber, startPosition + 1});
    if (bitmask & 0b010) hits.push_back({stripNumber, startPosition + 2});
    if (bitmask & 0b100) hits.push_back({stripNumber, startPosition + 3});
    return hits;
}

std::vector<Hit> parseCluster(const std::string& binary) {
    std::bitset<16> bits(binary);
    int stripNumber = (bits >> 11).to_ulong();
    int startPosition = ((bits << 5) >> 8).to_ulong();
    int sizeBitmask = static_cast<int>(((bits << 13) >> 13).to_ulong());
    return decodeSize(sizeBitmask, startPosition, stripNumber);
}

void mergeAndPrintClusters(const std::vector<Hit>& hits) {
    if (hits.empty()) return;

    // Sort hits using the custom comparator
    std::vector<Hit> sortedHits = hits; // Copy to sort
    std::sort(sortedHits.begin(), sortedHits.end(), HitComparator());

    // Merge adjacent hits into clusters
    std::vector<std::vector<Hit>> clusters;
    for (const auto& hit : sortedHits) {
        if (clusters.empty() || hit.stripNumber != clusters.back().back().stripNumber || hit.position > clusters.back().back().position + 1) {
            clusters.push_back({hit}); // Start a new cluster
        } else {
            clusters.back().push_back(hit); // Add to the existing cluster
        }
    }

    // Print clusters
    std::cout << "Final Merged Clusters:" << std::endl;
    for (const auto& cluster : clusters) {
        if (!cluster.empty()) {
            std::bitset<4> binaryStrip(cluster.front().stripNumber);
            std::bitset<8> binaryStart(cluster.front().position);
            std::bitset<3> binarySize(cluster.size() - 1); // Convert size to 3-bit bitmask for output
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
