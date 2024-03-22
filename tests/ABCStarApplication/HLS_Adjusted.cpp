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

void processHits(ap_uint<16> inputBinaries[MAX_HITS], int inputHitCount, Hit outputClusters[MAX_CLUSTERS], int& outputClusterCount) {
    #pragma HLS INTERFACE s_axilite port=return
    #pragma HLS INTERFACE s_axilite port=inputHitCount
    #pragma HLS INTERFACE s_axilite port=outputClusterCount
    #pragma HLS INTERFACE m_axi depth=MAX_HITS port=inputBinaries
    #pragma HLS INTERFACE m_axi depth=MAX_CLUSTERS port=outputClusters
    
    Hit hits[MAX_HITS];
    #pragma HLS ARRAY_PARTITION variable=hits complete dim=1
    
    int hitCount = 0;

    // Decode each binary input into hits
    DecodeLoop: for (int i = 0; i < inputHitCount; ++i) {
        #pragma HLS PIPELINE II=1
        const ap_uint<16> inputBinary = inputBinaries[i];
        const ap_uint<MODULE_NUMBER_BITS> moduleNumber = inputBinary >> (16 - MODULE_NUMBER_BITS);
        const ap_uint<POSITION_BITS> seedPosition = inputBinary & ((1 << POSITION_BITS) - 1);
        const ap_uint<3> sizeBitmask = (inputBinary >> POSITION_BITS) & 0x7;

        for (ap_uint<2> j = 0; j < 3; ++j) {
            if (sizeBitmask[j]) {
                const ap_uint<POSITION_BITS> hitPosition = seedPosition + (j + 1);
                if (hitCount < MAX_HITS) {
                    hits[hitCount++] = {moduleNumber, hitPosition};
                }
            }
        }
    }

    outputClusterCount = 0;
    ap_uint<MODULE_NUMBER_BITS> lastModuleNumber = 0;
    ap_uint<POSITION_BITS> lastPosition = 0;
    bool isFirstHit = true; // Introduced to handle the first hit separately

    // Process 'hits' array to form clusters, optimized to reduce dependencies
    ClusterFormationLoop: for (int i = 0; i < hitCount; ++i) {
        #pragma HLS PIPELINE II=1
        bool isNewCluster = false;
        
        // Check if it is the first hit or if the current hit does not belong to the current cluster
        if (isFirstHit || hits[i].moduleNumber != lastModuleNumber || hits[i].position != lastPosition + 1) {
            isNewCluster = true;
            isFirstHit = false; // Reset for subsequent iterations
        }

        // If it's a new cluster, increase the count and store the hit
        if (isNewCluster) {
            outputClusterCount++;
        }

        // Ensure we don't exceed the cluster limit
        if (outputClusterCount <= MAX_CLUSTERS) {
            outputClusters[outputClusterCount - 1] = hits[i];
        }

        // Update for next iteration
        lastModuleNumber = hits[i].moduleNumber;
        lastPosition = hits[i].position;
    }
}
