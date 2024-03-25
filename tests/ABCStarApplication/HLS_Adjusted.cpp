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
    // Interface pragmas are commented out as before
    
    Hit hits[MAX_HITS]; // Buffer to store decoded hits
    ap_uint<1> newClusterStart[MAX_HITS] = {0}; // Indicates the start of a new cluster
    int hitCount = 0; // Total number of hits after decoding

    // Decode input binaries into hits and determine cluster starts
    DecodeLoop: for (int i = 0; i < inputHitCount; ++i) {
        #pragma HLS PIPELINE
        const ap_uint<16> inputBinary = inputBinaries[i];
        const ap_uint<MODULE_NUMBER_BITS> moduleNumber = inputBinary >> (16 - MODULE_NUMBER_BITS);
        const ap_uint<POSITION_BITS> seedPosition = inputBinary & ((1 << POSITION_BITS) - 1);
        const ap_uint<3> sizeBitmask = (inputBinary >> POSITION_BITS) & 0x7;

        // Manually unrolled loop for j=0
        if (sizeBitmask[0] && hitCount < MAX_HITS) {
            ap_uint<POSITION_BITS> hitPosition = seedPosition + 1;
            hits[hitCount] = {moduleNumber, hitPosition};
            if (hitCount == 0 || !(moduleNumber == hits[hitCount-1].moduleNumber && hitPosition == hits[hitCount-1].position + 1)) {
                newClusterStart[hitCount] = 1;
            }
            hitCount++;
        }
        // Manually unrolled loop for j=1
        if (sizeBitmask[1] && hitCount < MAX_HITS) {
            ap_uint<POSITION_BITS> hitPosition = seedPosition + 2;
            hits[hitCount] = {moduleNumber, hitPosition};
            if (hitCount == 0 || !(moduleNumber == hits[hitCount-1].moduleNumber && hitPosition == hits[hitCount-1].position + 1)) {
                newClusterStart[hitCount] = 1;
            }
            hitCount++;
        }
        // Manually unrolled loop for j=2
        if (sizeBitmask[2] && hitCount < MAX_HITS) {
            ap_uint<POSITION_BITS> hitPosition = seedPosition + 3;
            hits[hitCount] = {moduleNumber, hitPosition};
            if (hitCount == 0 || !(moduleNumber == hits[hitCount-1].moduleNumber && hitPosition == hits[hitCount-1].position + 1)) {
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
            // Increment cluster count at the start of a new cluster
            outputClusterCount++;
        }
        // Assign hit to the current cluster, ensuring we do not exceed the cluster array
        if (outputClusterCount > 0 && outputClusterCount <= MAX_CLUSTERS) {
            outputClusters[outputClusterCount - 1] = hits[i];
        }
    }
}
