#include <iostream>
#include <hls_stream.h>
#include <ap_int.h>

#define MAX_HITS 1024
#define MAX_CLUSTERS 128
#define STRIP_SIZE 126
#define STRIP_NUMBER_BITS 11
#define HIT_START_SHIFT 5
#define POSITION_BITS 8
#define SIZE_BITMASK_BITS 13

struct Hit {
    ap_uint<STRIP_NUMBER_BITS> stripNumber; // Use ap_uint for HLS bit-precision
    ap_uint<POSITION_BITS> position;
};

// Simplified bubble sort for HLS 
void sortHits(Hit hits[], int n) {
    bool swapped;
    do {
        swapped = false;
        for (int i = 1; i < n; ++i) {
            if (hits[i-1].stripNumber > hits[i].stripNumber ||
               (hits[i-1].stripNumber == hits[i].stripNumber && hits[i-1].position > hits[i].position)) {
                Hit temp = hits[i-1];
                hits[i-1] = hits[i];
                hits[i] = temp;
                swapped = true;
            }
        }
    } while (swapped);
}

// Decode the size bitmask and populate hits array
int decodeSize(ap_uint<16> bitmask, int &hitCount, Hit hits[], int stripNumber, int seedPosition) {
    hits[hitCount++] = {stripNumber, seedPosition};
    if (bitmask & 0b001) hits[hitCount++] = {stripNumber, seedPosition + 3};
    if (bitmask & 0b010) hits[hitCount++] = {stripNumber, seedPosition + 2};
    if (bitmask & 0b100) hits[hitCount++] = {stripNumber, seedPosition + 1};
    return hitCount;
}

// Parse cluster from binary string and fill hits array
int parseCluster(const ap_uint<16> &binary, Hit hits[], int hitCount) {
    ap_uint<STRIP_NUMBER_BITS> stripNumber = binary >> (16 - STRIP_NUMBER_BITS);
    ap_uint<POSITION_BITS> seedPosition = (binary << STRIP_NUMBER_BITS) >> (16 - POSITION_BITS);
    ap_uint<3> sizeBitmask = binary & 0x7; // Last 3 bits

    return decodeSize(sizeBitmask, hitCount, hits, stripNumber, seedPosition);
}

// Merge adjacent hits into clusters
int mergeClusters(Hit hits[], int hitCount, Hit clusters[MAX_CLUSTERS][MAX_HITS], int clusterSizes[MAX_CLUSTERS]) {
    int clusterCount = 0;
    if (hitCount == 0) return 0;

    int size = 0;
    clusters[0][size++] = hits[0]; // Initialize the first cluster with the first hit

    for (int i = 1; i < hitCount; ++i) {
        bool isNewCluster = hits[i].stripNumber != hits[i-1].stripNumber || hits[i].position != hits[i-1].position + 1;

        if (isNewCluster) {
            clusterSizes[clusterCount++] = size; // Save the size of the current cluster
            size = 0; // Reset size for the new cluster
            if (clusterCount >= MAX_CLUSTERS) break; // Prevent exceeding the maximum number of clusters
        }

        clusters[clusterCount][size++] = hits[i]; // Add hit to the current or new cluster
    }

    clusterSizes[clusterCount++] = size; // Save the size of the last cluster
    return clusterCount;
}

// Print clusters - Format and specifics may need adjustment for HLS synthesis
void printClusters(Hit clusters[MAX_CLUSTERS][MAX_HITS], int clusterSizes[MAX_CLUSTERS], int clusterCount) {
    std::cout << "Final Merged Clusters:" << std::endl;
    for (int i = 0; i < clusterCount; ++i) {
        std::cout << "Cluster " << i << ": ";
        for (int j = 0; j < clusterSizes[i]; ++j) {
            std::cout << "(" << clusters[i][j].stripNumber << ", " << clusters[i][j].position << ") ";
        }
        std::cout << std::endl;
    }
}

int main() {
    // Input processing and function calls need to be adapted for HLS synthesis
    // This includes reading from streams or fixed-size arrays instead of std::cin
    // and outputting to streams or external interfaces.

    Hit hits[MAX_HITS];
    Hit clusters[MAX_CLUSTERS][MAX_HITS];
    int clusterSizes[MAX_CLUSTERS] = {0};
    int hitCount = 0;

    // Example: parseCluster calls and hit merging
    // hitCount = parseCluster(inputBinary, hits, hitCount);
    // Additional parseCluster calls as needed...

    sortHits(hits, hitCount); // Sort hits before merging
    int clusterCount = mergeClusters(hits, hitCount, clusters, clusterSizes); // Merge hits into clusters

    // printClusters(clusters, clusterSizes, clusterCount); // Printing is for demonstration and may not be synthesizable

    return 0;
}
