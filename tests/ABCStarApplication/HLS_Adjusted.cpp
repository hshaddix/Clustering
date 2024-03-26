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

// Function to process hits and cluster them
void processHits(ap_uint<16> inputBinaries[MAX_HITS], int inputHitCount, Hit outputClusters[MAX_CLUSTERS], int& outputClusterCount) {
    
    Hit hits[MAX_HITS]; // Buffer to store decoded hits
    ap_uint<1> newClusterStart[MAX_HITS] = {0}; // Indicates the start of a new cluster
    int hitCount = 0; // Total number of hits after decoding

    // Decode input binaries into hits and determine cluster starts
    DecodeLoop: for (int i = 0; i < inputHitCount; ++i) {
        #pragma HLS PIPELINE
        ap_uint<16> inputBinary = inputBinaries[i];
        ap_uint<MODULE_NUMBER_BITS> moduleNumber = inputBinary >> (16 - MODULE_NUMBER_BITS);
        ap_uint<POSITION_BITS> seedPosition = inputBinary & ((1 << POSITION_BITS) - 1);
        ap_uint<3> sizeBitmask = (inputBinary >> POSITION_BITS) & 0x7;

        int previousHitCount = hitCount;

        switch(sizeBitmask.to_uint()) {
            case 1: // 001
                hits[hitCount++] = {moduleNumber, seedPosition + 3};
                break;
            case 2: // 010
                hits[hitCount++] = {moduleNumber, seedPosition + 2};
                break;
            case 3: // 011
                hits[hitCount++] = {moduleNumber, seedPosition + 2};
                hits[hitCount++] = {moduleNumber, seedPosition + 3};
                break;
            case 4: // 100
                hits[hitCount++] = {moduleNumber, seedPosition + 1};
                break;
            case 5: // 101
                hits[hitCount++] = {moduleNumber, seedPosition + 1};
                hits[hitCount++] = {moduleNumber, seedPosition + 3};
                break;
            case 6: // 110
                hits[hitCount++] = {moduleNumber, seedPosition + 1};
                hits[hitCount++] = {moduleNumber, seedPosition + 2};
                break;
            case 7: // 111
                hits[hitCount++] = {moduleNumber, seedPosition + 1};
                hits[hitCount++] = {moduleNumber, seedPosition + 2};
                hits[hitCount++] = {moduleNumber, seedPosition + 3};
                break;
            default: // 000 and any other unexpected case
                break;
        }

        // Check adjacency and update newClusterStart for the newly added hits
        for (int j = previousHitCount; j < hitCount; ++j) {
            if (j == 0 || hits[j].moduleNumber != hits[j-1].moduleNumber || hits[j].position != hits[j-1].position + 1) {
                newClusterStart[j] = 1;
                outputClusterCount++;
            }
        }
    }

    // Assign hits to clusters based on pre-calculated flags
    int clusterIndex = -1;
    for (int i = 0; i < hitCount; ++i) {
        #pragma HLS PIPELINE 
        if (newClusterStart[i]) {
            clusterIndex++;
        }
        if (clusterIndex < MAX_CLUSTERS) {
            outputClusters[clusterIndex] = hits[i];
        }
    }
}
