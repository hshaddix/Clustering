// HLS_Adjusted_Attempt1.cpp
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
    #pragma HLS ARRAY_PARTITION variable=inputBinaries complete dim=1
    #pragma HLS ARRAY_PARTITION variable=outputClusters complete dim=1

    Hit hits[MAX_HITS];
    int hitCount = 0;

    for (int i = 0; i < inputHitCount; ++i) {
        #pragma HLS PIPELINE II=1
        ap_uint<MODULE_NUMBER_BITS> moduleNumber = inputBinaries[i] >> (16 - MODULE_NUMBER_BITS);
        ap_uint<POSITION_BITS> seedPosition = (inputBinaries[i] & ((1 << POSITION_BITS) - 1));
        ap_uint<3> sizeBitmask = (inputBinaries[i] >> POSITION_BITS) & 0x7;

        if (sizeBitmask[0] && hitCount < MAX_HITS) hits[hitCount++] = {moduleNumber, seedPosition + 1};
        if (sizeBitmask[1] && hitCount < MAX_HITS) hits[hitCount++] = {moduleNumber, seedPosition + 2};
        if (sizeBitmask[2] && hitCount < MAX_HITS) hits[hitCount++] = {moduleNumber, seedPosition + 3};
    }

    outputClusterCount = 0;

    for (int i = 0, currentClusterIndex = -1; i < hitCount; ++i) {
        #pragma HLS PIPELINE II=1
        bool isNewCluster = (i == 0) || !(hits[i].moduleNumber == hits[i-1].moduleNumber && hits[i].position == hits[i-1].position + 1);

        if (isNewCluster) {
            currentClusterIndex++;
            if (currentClusterIndex < MAX_CLUSTERS) {
                outputClusters[currentClusterIndex] = hits[i];
                outputClusterCount++;
            }
        }
    }
}
