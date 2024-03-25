#include <ap_int.h>
#include <hls_stream.h>

#define MAX_HITS 1024
#define MAX_CLUSTERS 128
#define MODULE_NUMBER_BITS 11
#define POSITION_BITS 8

struct Hit {
    ap_uint<MODULE_NUMBER_BITS> moduleNumber;
    ap_uint<POSITION_BITS> position;
};

// Function to process hits and cluster them
void processHits(ap_uint<16> inputBinaries[MAX_HITS], int inputHitCount, Hit outputClusters[MAX_CLUSTERS], int& outputClusterCount) {
    Hit hits[MAX_HITS]; // Buffer to store decoded hits
    ap_uint<1> newClusterStart[MAX_HITS] = {0}; // Indicates the start of a new cluster
    int hitCount = 0; // Total number of hits after decoding

    DecodeLoop: for (int i = 0; i < inputHitCount; ++i) {
        #pragma HLS PIPELINE 
        const ap_uint<16> inputBinary = inputBinaries[i];
        ap_uint<MODULE_NUMBER_BITS> moduleNumber = inputBinary >> (16 - MODULE_NUMBER_BITS);
        ap_uint<POSITION_BITS> seedPosition = inputBinary & ((1 << POSITION_BITS) - 1);
        ap_uint<3> sizeBitmask = (inputBinary >> POSITION_BITS) & 0x7;

        // Pre-calculate hit positions based on the sizeBitmask
        Hit potentialHits[3];
        int numHitsToAdd = 0;

        for (int j = 0; j < 3; ++j) {
            if (sizeBitmask[j] && hitCount + numHitsToAdd < MAX_HITS) {
                potentialHits[numHitsToAdd++] = {moduleNumber, seedPosition + (j + 1)};
            }
        }

        // Add all pre-calculated hit positions to the hits array
        for (int j = 0; j < numHitsToAdd; ++j) {
            hits[hitCount] = potentialHits[j];
            // Determine new cluster starts here as needed
            if (hitCount == 0 || !(potentialHits[j].moduleNumber == hits[hitCount-1].moduleNumber && potentialHits[j].position == hits[hitCount-1].position + 1)) {
                newClusterStart[hitCount] = 1;
            }
            hitCount++;
        }
    }

    // Initialize cluster count based on whether there are any hits
    outputClusterCount = (hitCount > 0) ? 1 : 0;

    // Assign hits to clusters based on pre-calculated flags
    ClusterAssignmentLoop: for (int i = 0; i < hitCount; ++i) {
        #pragma HLS PIPELINE
        if (newClusterStart[i] && i > 0 && outputClusterCount < MAX_CLUSTERS) {
            outputClusterCount++;
        }
        if (outputClusterCount > 0 && outputClusterCount <= MAX_CLUSTERS) {
            outputClusters[outputClusterCount - 1] = hits[i];
        }
    }
}
