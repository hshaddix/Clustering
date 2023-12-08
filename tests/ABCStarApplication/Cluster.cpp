#include <iostream>
#include <vector>
#include <bitset>
#include <string>

const int BITS_FOR_ABCSTAR = 4;
const int BITS_FOR_CHANNEL = 8;
const int BITS_FOR_NEXT_STRIPS = 3;
const int TOTAL_BITS = 16;

struct Cluster {
    int abcstarNumber;
    int startStrip;
    int length;
};

std::vector<Cluster> processClusters(const std::vector<std::string>& inputClusters);
void printClusters(const std::vector<Cluster>& clusters);

int main() {
    std::vector<std::string> inputClusters;
    std::string clusterBinary;

    // Read input clusters in binary format
    while (std::cin >> clusterBinary) {
        inputClusters.push_back(clusterBinary);
    }

    auto processedClusters = processClusters(inputClusters);
    printClusters(processedClusters);

    return 0;
}

std::vector<Cluster> processClusters(const std::vector<std::string>& inputClusters) {
    std::vector<Cluster> processedClusters;
    Cluster currentCluster;
    bool isFirstCluster = true;

    for (const auto& clusterBinary : inputClusters) {
        std::bitset<TOTAL_BITS> clusterBits(clusterBinary);
        int abcstarNumber = (clusterBits >> (BITS_FOR_CHANNEL + BITS_FOR_NEXT_STRIPS)).to_ulong();
        int startStrip = ((clusterBits << BITS_FOR_ABCSTAR) >> (BITS_FOR_ABCSTAR + BITS_FOR_NEXT_STRIPS)).to_ulong();
        int nextStrips = (clusterBits & std::bitset<TOTAL_BITS>(0b111)).to_ulong();

        int length = 1; // First strip is always hit
        for (int i = 0; i < BITS_FOR_NEXT_STRIPS; ++i) {
            if (nextStrips & (1 << (BITS_FOR_NEXT_STRIPS - 1 - i))) {
                length++;
            }
        }

        if (isFirstCluster || currentCluster.abcstarNumber != abcstarNumber || currentCluster.startStrip + currentCluster.length != startStrip) {
            if (!isFirstCluster) {
                processedClusters.push_back(currentCluster);
            }
            currentCluster = {abcstarNumber, startStrip, length};
            isFirstCluster = false;
        } else {
            currentCluster.length += length;
        }
    }

    if (!isFirstCluster) {
        processedClusters.push_back(currentCluster);
    }

    return processedClusters;
}

void printClusters(const std::vector<Cluster>& clusters) {
    for (const auto& cluster : clusters) {
        std::cout << "ABCstar Number: " << cluster.abcstarNumber
                  << ", Start Strip: " << cluster.startStrip
                  << ", Length: " << cluster.length << std::endl;
    }
}
