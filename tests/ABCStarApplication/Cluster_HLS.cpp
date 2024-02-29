#include <ap_int.h>
#include <hls_stream.h>

#define MAX_HITS 1024
#define MAX_CLUSTERS 128
#define STRIP_SIZE 126
#define STRIP_NUMBER_BITS 11
#define POSITION_BITS 8

struct Hit {
    ap_uint<STRIP_NUMBER_BITS> stripNumber;
    ap_uint<POSITION_BITS> position;
};

// Declare the top-level function for HLS synthesis
void processHits(ap_uint<16> inputBinaries[MAX_HITS], int inputHitCount, Hit outputClusters[MAX_CLUSTERS], int& outputClusterCount);

// Function to decode the size bitmask and add hits based on the seed position
void decodeSize(ap_uint<3> bitmask, int seedPosition, int stripNumber, Hit hits[MAX_HITS], int& hitCount) {
    #pragma HLS INLINE
    Hit initialHit = {stripNumber, seedPosition};
    hits[hitCount++] = initialHit; // Always include the seed hit

    // Decode additional hits based on the bitmask
    if (bitmask[0]) hits[hitCount++] = {stripNumber, (ap_uint<POSITION_BITS>)(seedPosition + 1)};
    if (bitmask[1]) hits[hitCount++] = {stripNumber, (ap_uint<POSITION_BITS>)(seedPosition + 2)};
    if (bitmask[2]) hits[hitCount++] = {stripNumber, (ap_uint<POSITION_BITS>)(seedPosition + 3)};
}

// Utility function to sort hits by strip number and position
void sortHits(Hit hits[MAX_HITS], int n) {
    Hit temp;
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            if (hits[i].stripNumber > hits[j].stripNumber || (hits[i].stripNumber == hits[j].stripNumber && hits[i].position > hits[j].position)) {
                temp = hits[i];
                hits[i] = hits[j];
                hits[j] = temp;
            }
        }
    }
}

// Function to merge adjacent hits into clusters
void mergeClusters(Hit hits[MAX_HITS], int hitCount, Hit outputClusters[MAX_CLUSTERS], int& outputClusterCount) {
    int currentClusterStart = 0;
    outputClusterCount = 0;

    for (int i = 1; i < hitCount; i++) {
        bool adjacent = (hits[i].stripNumber == hits[i-1].stripNumber && hits[i].position == hits[i-1].position + 1);
        bool newStrip = hits[i].stripNumber != hits[i-1].stripNumber;

        if (!adjacent || newStrip) {
            outputClusters[outputClusterCount++] = hits[currentClusterStart]; // Save the start of the cluster
            currentClusterStart = i; // New cluster starts
        }
    }
    // Save the last cluster
    if (hitCount > 0) {
        outputClusters[outputClusterCount++] = hits[currentClusterStart];
    }
}

// Top-level function definition
void processHits(ap_uint<16> inputBinaries[MAX_HITS], int inputHitCount, Hit outputClusters[MAX_CLUSTERS], int& outputClusterCount) {
    #pragma HLS INTERFACE s_axilite port=return
    #pragma HLS INTERFACE s_axilite port=inputHitCount
    #pragma HLS INTERFACE s_axilite port=outputClusterCount
    #pragma HLS INTERFACE m_axi depth=MAX_HITS port=inputBinaries
    #pragma HLS INTERFACE m_axi depth=MAX_CLUSTERS port=outputClusters

    Hit hits[MAX_HITS];
    int hitCount = 0;

    // Decode each binary input into hits
    for (int i = 0; i < inputHitCount; i++) {
        #pragma HLS PIPELINE II=1
        ap_uint<STRIP_NUMBER_BITS> stripNumber = inputBinaries[i] >> (16 - STRIP_NUMBER_BITS);
        ap_uint<POSITION_BITS> seedPosition = (inputBinaries[i] & ((1 << POSITION_BITS) - 1));
        ap_uint<3> sizeBitmask = (inputBinaries[i] >> POSITION_BITS) & 0x7;
        decodeSize(sizeBitmask, seedPosition.to_int(), stripNumber.to_int(), hits, hitCount);
    }

    // Sort the hits before clustering
    sortHits(hits, hitCount);

    // Merge adjacent hits into clusters
    mergeClusters(hits, hitCount, outputClusters, outputClusterCount);
}
