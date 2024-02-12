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

// Function to decode the bitmask into a vector of hit positions
std::vector<Hit> decodeSize(int bitmask, int startPosition, int stripNumber) {
    std::vector<Hit> hits;
    // Direct inclusion of the seed hit for all cases except '000'
    if (bitmask != 0b000) {
        hits.push_back({stripNumber, startPosition});
    }
    // Decoding the bitmask to identify additional hits or clusters
    if (bitmask & 0b001) hits.push_back({stripNumber, startPosition + 1});
    if (bitmask & 0b010) hits.push_back({stripNumber, startPosition + 2});
    if (bitmask & 0b100) hits.push_back({stripNumber, startPosition + 3});

    return hits;
}

// Parses the binary string into a vector of Hit structures
std::vector<Hit> parseCluster(const std::string& binary) {
    std::bitset<16> bits(binary);
    int stripNumber = (bits >> 11).to_ulong();
    int startPosition = ((bits << 5) >> 8).to_ulong();
    int sizeBitmask = static_cast<int>(((bits << 13) >> 13).to_ulong());

    return decodeSize(sizeBitmask, startPosition, stripNumber);
}

// Merges adjacent hits into clusters and prints the final clusters
void mergeAndPrintClusters(const std::vector<Hit>& hits) {
    if (hits.empty()) return;

    std::sort(hits.begin(), hits.end(), [](const Hit& a, const Hit& b) {
        return a.position < b.position || (a.position == b.position && a.stripNumber < b.stripNumber);
    });

    std::vector<std::vector<Hit>> clusters;
    for (const auto& hit : hits) {
        if (!clusters.empty() && 
            clusters.back().back().stripNumber == hit.stripNumber && 
            clusters.back().back().position + 1 == hit.position) {
            clusters.back().push_back(hit);
        } else if (!clusters.empty() && 
                   clusters.back().back().stripNumber == hit.stripNumber &&
                   clusters.back().back().position == hit.position) {
            // Do not add if it's the same position as the last hit (handles '000' case and duplicates)
            continue;
        } else {
            clusters.push_back({hit});
        }
    }

    // Printing clusters
    std::cout << "Final Merged Clusters:" << std::endl;
    for (const auto& cluster : clusters) {
        if (cluster.empty()) continue;
        std::bitset<4> binaryStrip(cluster.front().stripNumber);
        std::bitset<8> binaryStart(cluster.front().position);
        int size = cluster.size();
        std::bitset<3> binarySize(size - 1); // Adjusting size to match expected output format

        // Output format: 0 + Strip(4 bits) + Start Position(8 bits) + Size(3 bits)
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

    mergeAndPrintClusters(allHits);

    return 0;
}
