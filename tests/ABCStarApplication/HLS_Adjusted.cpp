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
    int hitCount = 0;

    // Decode each binary input into hits
    for (int i = 0; i < inputHitCount; ++i) {
        #pragma HLS PIPELINE II=1
        const ap_uint<MODULE_NUMBER_BITS> moduleNumber = inputBinaries[i] >> (16 - MODULE_NUMBER_BITS);
        const ap_uint<POSITION_BITS> seedPosition = inputBinaries[i] & ((1 << POSITION_BITS) - 1);
        const ap_uint<3> sizeBitmask = (inputBinaries[i] >> POSITION_BITS) & 0x7;

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
    ap_uint<POSITION_BITS> lastPosition = -1; // Initialize to an impossible position value

    // Simplified Cluster Formation Loop to eliminate dependency issues
    for (int i = 0; i < hitCount; ++i) {
        #pragma HLS PIPELINE II=1
        const Hit& currentHit = hits[i];
        bool isNewCluster = true;

        // Determine if the current hit continues the last cluster or starts a new one
        if (i > 0) {
            isNewCluster = !(currentHit.moduleNumber == lastModuleNumber && currentHit.position == lastPosition + 1);
        }

        if (isNewCluster) {
            // Start a new cluster if we're not continuing the last one
            if (outputClusterCount < MAX_CLUSTERS) {
                outputClusters[outputClusterCount++] = currentHit;
            }
        } else {
            // Update the last hit of the current cluster if we're continuing
            if (outputClusterCount > 0) {
                outputClusters[outputClusterCount - 1] = currentHit;
            }
        }

        // Update the reference for the next iteration
        lastModuleNumber = currentHit.moduleNumber;
        lastPosition = currentHit.position;
    }
}
