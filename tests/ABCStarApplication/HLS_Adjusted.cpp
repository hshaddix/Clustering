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

// HLS synthesis top-level function declaration
void processHits(ap_uint<16> inputBinaries[MAX_HITS], int inputHitCount, Hit outputClusters[MAX_CLUSTERS], int& outputClusterCount) {
    #pragma HLS INTERFACE s_axilite port=return
    #pragma HLS INTERFACE s_axilite port=inputHitCount
    #pragma HLS INTERFACE s_axilite port=outputClusterCount
    #pragma HLS INTERFACE m_axi depth=MAX_HITS port=inputBinaries
    #pragma HLS INTERFACE m_axi depth=MAX_CLUSTERS port=outputClusters

    Hit hits[MAX_HITS];
    int hitCount = 0; // Count of decoded hits

    // Loop to decode each binary input into hits
    DecodeLoop: for (int i = 0; i < inputHitCount; ++i) {
        #pragma HLS PIPELINE II=1
        ap_uint<MODULE_NUMBER_BITS> moduleNumber = inputBinaries[i] >> (16 - MODULE_NUMBER_BITS);
        ap_uint<POSITION_BITS> seedPosition = (inputBinaries[i] & ((1 << POSITION_BITS) - 1));
        ap_uint<3> sizeBitmask = (inputBinaries[i] >> POSITION_BITS) & 0x7;

        // Direct assignment to the hits array within constraints
        if (sizeBitmask[0] && hitCount < MAX_HITS) hits[hitCount++] = {moduleNumber, seedPosition + 3};
        if (sizeBitmask[1] && hitCount < MAX_HITS) hits[hitCount++] = {moduleNumber, seedPosition + 2};
        if (sizeBitmask[2] && hitCount < MAX_HITS) hits[hitCount++] = {moduleNumber, seedPosition + 1};
    }

    outputClusterCount = 0; // Initialize final cluster count
    int currentClusterIndex = -1; // Initialize to -1 to correctly handle the first hit as a new cluster

    // Loop to merge adjacent hits into clusters
    MergeLoop: for (int i = 0; i < hitCount; ++i) {
        #pragma HLS PIPELINE II=1
        bool isNewCluster = (i == 0) || !(hits[i].moduleNumber == hits[i-1].moduleNumber && hits[i].position == hits[i-1].position + 1);
        
        if (isNewCluster) {
            // Increment cluster index and count only if within bounds
            if (currentClusterIndex < MAX_CLUSTERS - 1) {
                currentClusterIndex++;
                outputClusterCount++; // Increment the cluster count for each new cluster
            }
        }

        if (currentClusterIndex < MAX_CLUSTERS) {
            outputClusters[currentClusterIndex] = hits[i]; // Assign hit to the current cluster
        }
    }
}
