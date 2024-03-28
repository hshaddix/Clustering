#include <ap_int.h>
#include <hls_stream.h>

#define MAX_HITS 1024
#define MAX_CLUSTERS 128
#define ABCStar_ID_BITS 11
#define POSITION_BITS 8
#define ABCStar_SIZE 128 // Assuming a fixed ABCStar size of 128 positions.

struct Hit {
    ap_uint<ABCStar_ID_BITS> ABCStarID;
    ap_uint<POSITION_BITS> position;
};

// Function to process hits and cluster them
void processHits(ap_uint<16> inputBinaries[MAX_HITS], int inputHitCount, Hit outputClusters[MAX_CLUSTERS], int& outputClusterCount) {
    Hit hits[MAX_HITS]; // Buffer to store decoded hits
    ap_uint<1> newClusterStart[MAX_HITS] = {0}; // Indicates the start of a new cluster
    int hitCount = 0; // Total number of hits after decoding
    outputClusterCount = 0; // Initialize output cluster count

    DecodeLoop: for (int i = 0; i < inputHitCount; ++i) {
        #pragma HLS PIPELINE
        ap_uint<16> inputBinary = inputBinaries[i];
        ap_uint<ABCStar_ID_BITS> ABCStarID = inputBinary >> (16 - ABCStar_ID_BITS);
        ap_uint<POSITION_BITS> seedPosition = inputBinary & ((1 << POSITION_BITS) - 1);
        ap_uint<3> sizeBitmask = (inputBinary >> POSITION_BITS) & 0x7;

        switch(sizeBitmask.to_uint()) {
            case 1: // 001
                hits[hitCount++] = {ABCStarID, seedPosition + 3};
                break;
            case 2: // 010
                hits[hitCount++] = {ABCStarID, seedPosition + 2};
                break;
            case 3: // 011
                hits[hitCount++] = {ABCStarID, seedPosition + 2};
                hits[hitCount++] = {ABCStarID, seedPosition + 3};
                break;
            case 4: // 100
                hits[hitCount++] = {ABCStarID, seedPosition + 1};
                break;
            case 5: // 101
                hits[hitCount++] = {ABCStarID, seedPosition + 1};
                hits[hitCount++] = {ABCStarID, seedPosition + 3};
                break;
            case 6: // 110
                hits[hitCount++] = {ABCStarID, seedPosition + 1};
                hits[hitCount++] = {ABCStarID, seedPosition + 2};
                break;
            case 7: // 111
                hits[hitCount++] = {ABCStarID, seedPosition + 1};
                hits[hitCount++] = {ABCStarID, seedPosition + 2};
                hits[hitCount++] = {ABCStarID, seedPosition + 3};
                break;
            default: // 000 and any other unexpected case
                break;
        }
    }

    // Determine cluster starts based on adjacency, including ABCStar boundaries
    if (hitCount > 0) {
        newClusterStart[0] = 1;
        outputClusterCount = 1;
        for (int i = 1; i < hitCount; ++i) {
            #pragma HLS PIPELINE
            bool isAdjacent = false;
            // Check adjacency within the same ABCStar
            if (hits[i].ABCStarID == hits[i-1].ABCStarID && hits[i].position == hits[i-1].position + 1) {
                isAdjacent = true;
            }
            // Check adjacency across ABCStar boundaries
            if (hits[i].ABCStarID == hits[i-1].ABCStarID + 1 && hits[i-1].position == ABCStar_SIZE - 1 && hits[i].position == 0) {
                isAdjacent = true;
            }
            newClusterStart[i] = !isAdjacent;
            outputClusterCount += !isAdjacent ? 1 : 0;
        }
    }

    // Populate outputClusters based on newClusterStart flags
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
