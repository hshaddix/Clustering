\#include <ap_int.h>
#include <hls_stream.h>

#define MAX_HITS 1024
#define MAX_CLUSTERS 128
#define STRIP_SIZE 126
#define STRIP_NUMBER_BITS 11
#define POSITION_BITS 8

struct Hit {
    ap_uint<STRIP_NUMBER_BITS> stripNumber;
    ap_uint<POSITION_BITS> position;
};

void processHits(ap_uint<16> inputBinaries[MAX_HITS], int inputHitCount, Hit outputClusters[MAX_CLUSTERS], int& outputClusterCount) {
    #pragma HLS INTERFACE s_axilite port=return
    #pragma HLS INTERFACE s_axilite port=inputHitCount
    #pragma HLS INTERFACE s_axilite port=outputClusterCount
    #pragma HLS INTERFACE m_axi depth=MAX_HITS port=inputBinaries
    #pragma HLS INTERFACE m_axi depth=MAX_CLUSTERS port=outputClusters

    Hit hits[MAX_HITS];
    int hitCount = 0; // Initialize the hit count

    // Iterate over each input binary and decode hits
    DECODE_LOOP: for (int i = 0; i < inputHitCount; ++i) {
        #pragma HLS PIPELINE II=1
        ap_uint<STRIP_NUMBER_BITS> stripNumber = inputBinaries[i] >> (16 - STRIP_NUMBER_BITS);
        ap_uint<POSITION_BITS> seedPosition = (inputBinaries[i] & ((1 << POSITION_BITS) - 1));
        ap_uint<3> sizeBitmask = (inputBinaries[i] >> POSITION_BITS) & 0x7;

        // Always include the seed hit
        hits[hitCount++] = {stripNumber, seedPosition};

        // Decode additional hits based on the bitmask
        if (bitmask[0]) hits[hitCount++] = {stripNumber, seedPosition + 3};
        if (bitmask[1]) hits[hitCount++] = {stripNumber, seedPosition + 2};
        if (bitmask[2]) hits[hitCount++] = {stripNumber, seedPosition + 1};
    }

    // Initialize output cluster count
    outputClusterCount = 0;

    // Merge adjacent hits into clusters
    MERGE_LOOP: for (int i = 1; i < hitCount; ++i) {
        #pragma HLS PIPELINE II=1
        bool adjacent = (hits[i].stripNumber == hits[i-1].stripNumber && hits[i].position == hits[i-1].position + 1);
        bool newStrip = hits[i].stripNumber != hits[i-1].stripNumber;

        if (!adjacent || newStrip) {
            outputClusters[outputClusterCount++] = hits[i-1]; // Save the start of the cluster
        }

        // Special case for the last hit
        if (i == hitCount - 1) {
            outputClusters[outputClusterCount++] = hits[i];
        }
    }
}
