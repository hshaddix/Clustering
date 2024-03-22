#include <ap_int.h>
#include <hls_stream.h>

#define MAX_HITS 1024
#define MAX_CLUSTERS 128
#define STRIP_SIZE 126
#define MODULE_NUMBER_BITS 11
#define POSITION_BITS 8

struct Hit {
    ap_uint<MODULE_NUMBER_BITS> moduleNumber;
    ap_uint<POSITION_BITS> position;
};

// Top function prototype declaration
void processHits(ap_uint<16> inputBinaries[MAX_HITS], int inputHitCount, Hit outputClusters[MAX_CLUSTERS], int& outputClusterCount);

void decodeSize(ap_uint<3> bitmask, int seedPosition, int moduleNumber, Hit hits[MAX_HITS], int& hitCount) {
    #pragma HLS INLINE
    Hit tempHits[3]; // Temporary buffer to hold hits for a single binary input
    int tempHitCount = 0;
    
    if (bitmask[0]) tempHits[tempHitCount++] = {moduleNumber, seedPosition + 3};
    if (bitmask[1]) tempHits[tempHitCount++] = {moduleNumber, seedPosition + 2};
    if (bitmask[2]) tempHits[tempHitCount++] = {moduleNumber, seedPosition + 1};

    // Append decoded hits to the main hits array
    for (int i = 0; i < tempHitCount; i++) {
        #pragma HLS UNROLL
        hits[hitCount++] = tempHits[i];
    }
}

void mergeClusters(Hit hits[MAX_HITS], int hitCount, Hit outputClusters[MAX_CLUSTERS], int& finalClusterCount) {
    #pragma HLS INLINE
    finalClusterCount = 0; // Initialize final cluster count
    Hit previousHit = {0, 0}; // Store the previous hit to compare with the current hit

    for (int i = 0; i < hitCount; ++i) {
        #pragma HLS PIPELINE II=1
        bool isNewCluster = (i == 0) || !(hits[i].moduleNumber == previousHit.moduleNumber && hits[i].position == previousHit.position + 1);

        if (isNewCluster) {
            if (finalClusterCount < MAX_CLUSTERS) {
                outputClusters[finalClusterCount++] = hits[i]; // Assign hit to a new cluster
            }
        }
        previousHit = hits[i]; // Update the previous hit
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

    // Decode each binary input into hits
    for (int i = 0; i < inputHitCount; i++) {
        #pragma HLS PIPELINE II=1
        ap_uint<MODULE_NUMBER_BITS> moduleNumber = inputBinaries[i] >> (16 - MODULE_NUMBER_BITS);
        ap_uint<POSITION_BITS> seedPosition = (inputBinaries[i] & ((1 << POSITION_BITS) - 1));
        ap_uint<3> sizeBitmask = (inputBinaries[i] >> POSITION_BITS) & 0x7;

        decodeSize(sizeBitmask, seedPosition.to_int(), moduleNumber.to_int(), hits, hitCount);
    }

    // Merge adjacent hits into clusters
    mergeClusters(hits, hitCount, outputClusters, outputClusterCount);
}
