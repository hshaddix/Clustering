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

    DecodeLoop: for (int i = 0; i < inputHitCount; ++i) {
        #pragma HLS PIPELINE II=1
        const ap_uint<16> inputBinary = inputBinaries[i];
        const ap_uint<MODULE_NUMBER_BITS> moduleNumber = inputBinary >> (16 - MODULE_NUMBER_BITS);
        const ap_uint<POSITION_BITS> seedPosition = inputBinary & ((1 << POSITION_BITS) - 1);
        const ap_uint<3> sizeBitmask = (inputBinary >> POSITION_BITS) & 0x7;

        // Preprocess the bitmask to determine the positions of additional hits
        // This approach directly calculates each hit's position based on the bitmask without referring to previously calculated hits
        for (ap_uint<2> j = 0; j < 3; ++j) {
            if (sizeBitmask[j]) { // Check if the bit is set, indicating an adjacent hit
                const ap_uint<POSITION_BITS> hitPosition = seedPosition + (j + 1);
                if (hitCount < MAX_HITS) {
                    hits[hitCount++] = {moduleNumber, hitPosition};
                }
            }
        }
    }

    outputClusterCount = 0;
    Hit lastHit = {0xFFFFFFFF, 0xFFFFFFFF}; // Initialize with an invalid value

    ClusterFormationLoop: for (int i = 0; i < hitCount; ++i) {
        #pragma HLS PIPELINE II=1
        const bool isNewCluster = (i == 0) || !(hits[i].moduleNumber == lastHit.moduleNumber && hits[i].position == lastHit.position + 1);

        if (isNewCluster && outputClusterCount < MAX_CLUSTERS) {
            outputClusterCount++;
        }

        if (outputClusterCount > 0 && outputClusterCount <= MAX_CLUSTERS) {
            outputClusters[outputClusterCount - 1] = hits[i];
        }

        lastHit = hits[i]; // Update last hit for next iteration comparison
    }
}
