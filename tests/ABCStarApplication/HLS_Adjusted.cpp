#include <ap_int.h>
#include <hls_stream.h>

#define MAX_HITS 1024
#define MAX_CLUSTERS 128
#define STRIP_SIZE 126
#define MODULE_NUMBER_BITS 11
#define POSITION_BITS 8

struct Hit {
    ap_uint<MODULE_NUMBER_BITS> stripNumber;
    ap_uint<POSITION_BITS> position;
};

// Declare the top-level function for HLS synthesis
void processHits(ap_uint<16> inputBinaries[MAX_HITS], int inputHitCount, Hit outputClusters[MAX_CLUSTERS], int& outputClusterCount);

void mergeClusters(Hit hits[MAX_HITS], int hitCount, Hit outputClusters[MAX_CLUSTERS], int& finalClusterCount) {
    #pragma HLS INLINE
    finalClusterCount = 0; // Initialize final cluster count
    int currentClusterIndex = -1; // Initialize to -1 to correctly handle the first hit as a new cluster

    for (int i = 0; i < hitCount; ++i) {
        #pragma HLS PIPELINE II=1
        bool isNewCluster = (i == 0) || !(hits[i].stripNumber == hits[i-1].stripNumber && hits[i].position == hits[i-1].position + 1);
        if (isNewCluster) {
            currentClusterIndex++;
            finalClusterCount++; // Increment the cluster count for each new cluster
        }
        if (currentClusterIndex < MAX_CLUSTERS) {
            outputClusters[currentClusterIndex] = hits[i]; // Assign hit to the current cluster
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

    // Process each binary input to decode into hits
    for (int i = 0; i < inputHitCount; i++) {
        #pragma HLS PIPELINE II=1
        ap_uint<MODULE_NUMBER_BITS> stripNumber = inputBinaries[i] >> (16 - MODULE_NUMBER_BITS);
        ap_uint<POSITION_BITS> seedPosition = (inputBinaries[i] & ((1 << POSITION_BITS) - 1));
        ap_uint<3> sizeBitmask = (inputBinaries[i] >> POSITION_BITS) & 0x7;

        // Inline the decodeSize logic here
        if (sizeBitmask[0]) hits[hitCount++] = {stripNumber, seedPosition + 1};
        if (sizeBitmask[1]) hits[hitCount++] = {stripNumber, seedPosition + 2};
        if (sizeBitmask[2]) hits[hitCount++] = {stripNumber, seedPosition + 3};
    }

    // Merge adjacent hits into clusters
    mergeClusters(hits, hitCount, outputClusters, outputClusterCount);
}
