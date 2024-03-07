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

void decodeSize(ap_uint<3> bitmask, int seedPosition, int stripNumber, Hit hits[MAX_HITS], int& hitCount) {
    #pragma HLS INLINE
    if (bitmask[0]) hits[hitCount++] = {stripNumber, seedPosition + 3};
    if (bitmask[1]) hits[hitCount++] = {stripNumber, seedPosition + 2};
    if (bitmask[2]) hits[hitCount++] = {stripNumber, seedPosition + 1};
}

void mergeClusters(Hit hits[MAX_HITS], int hitCount, Hit outputClusters[MAX_CLUSTERS], int& finalClusterCount) {
    #pragma HLS INLINE
    int currentClusterIndex = 0; // Track the current index for output clusters
    finalClusterCount = 0; // Initialize final cluster count

    for (int i = 0; i < hitCount; ++i) {
        #pragma HLS PIPELINE II=1
        // Check for adjacency including strip continuity
        if (i > 0 && !(hits[i].stripNumber == hits[i-1].stripNumber && hits[i].position == hits[i-1].position + 1)) {
            // Not adjacent or on a new strip, increment cluster index
            currentClusterIndex++;
        }
        if (currentClusterIndex < MAX_CLUSTERS) {
            outputClusters[currentClusterIndex] = hits[i];
            if (i == 0 || currentClusterIndex != finalClusterCount) {
                // New cluster started
                finalClusterCount++;
            }
        }
    }
}

void processHits(ap_uint<16> inputBinaries[MAX_HITS], int inputHitCount, Hit outputClusters[MAX_CLUSTERS], int& outputClusterCount) {
    #pragma HLS INTERFACE s_axilite port=return
    #pragma HLS INTERFACE s_axilite port=inputHitCount
    #pragma HLS INTERFACE s_axilite port=outputClusterCount
    #pragma HLS INTERFACE m_axi depth=MAX_HITS port=inputBinaries
    #pragma HLS INTERFACE m_axi depth=MAX_CLUSTERS port=outputClusters

    Hit hits[MAX_HITS];
    int hitCount = 0; // Count of decoded hits

    for (int i = 0; i < inputHitCount; ++i) {
        #pragma HLS PIPELINE II=1
        ap_uint<STRIP_NUMBER_BITS> stripNumber = inputBinaries[i] >> (16 - STRIP_NUMBER_BITS);
        ap_uint<POSITION_BITS> seedPosition = (inputBinaries[i] & ((1 << POSITION_BITS) - 1));
        ap_uint<3> sizeBitmask = (inputBinaries[i] >> POSITION_BITS) & 0x7;

        decodeSize(sizeBitmask, seedPosition.to_int(), stripNumber.to_int(), hits, hitCount);
    }

    // As inputs are assumed to be sorted, no need to sort hits here
    mergeClusters(hits, hitCount, outputClusters, outputClusterCount);
}
