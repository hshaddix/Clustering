#include <ap_int.h>
#include <hls_stream.h>

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

int decodeSize(ap_uint<16> bitmask, int &hitCount, Hit hits[], int stripNumber, int seedPosition) {
    hits[hitCount++] = {stripNumber, seedPosition}; // Include the seed hit in the hits list.
    if (bitmask & 0b001) hits[hitCount++] = {stripNumber, seedPosition + 3};
    if (bitmask & 0b010) hits[hitCount++] = {stripNumber, seedPosition + 2};
    if (bitmask & 0b100) hits[hitCount++] = {stripNumber, seedPosition + 1};
    return hitCount;
}

int parseCluster(const ap_uint<16> &binary, Hit hits[], int hitCount) {
    ap_uint<STRIP_NUMBER_BITS> stripNumber = binary >> (16 - STRIP_NUMBER_BITS);
    ap_uint<POSITION_BITS> seedPosition = (binary << STRIP_NUMBER_BITS) >> (16 - POSITION_BITS);
    ap_uint<3> sizeBitmask = binary & 0x7; // Last 3 bits represent the size bitmask

    return decodeSize(sizeBitmask, hitCount, hits, stripNumber, seedPosition);
}

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

// Defined top function for HLS synthesis
void processHits(ap_uint<16> inputBinaries[MAX_HITS], int inputHitCount, Hit outputClusters[MAX_CLUSTERS][MAX_HITS], int& outputClusterCount) {
    #pragma HLS INTERFACE s_axilite port=return
    #pragma HLS INTERFACE s_axilite port=inputHitCount
    #pragma HLS INTERFACE s_axilite port=outputClusterCount
    #pragma HLS INTERFACE bram port=inputBinaries
    #pragma HLS INTERFACE bram port=outputClusters

    Hit hits[MAX_HITS];
    int hitCount = 0;
    int clusterSizes[MAX_CLUSTERS] = {0};

    // Iterate over each binary input and decode
    for (int i = 0; i < inputHitCount; ++i) {
        hitCount = parseCluster(inputBinaries[i], hits, hitCount);
        if(hitCount >= MAX_HITS) break; // Ensure hitCount does not exceed MAX_HITS
    }

    sortHits(hits, hitCount); // Now sort hits
    outputClusterCount = mergeClusters(hits, hitCount, outputClusters, clusterSizes); // Then merge clusters
}

int main() {
    // The main function is not used for synthesis and serves only for simulation purposes.
    // Define your test cases and simulate the `processHits` function here.
    return 0;
}
