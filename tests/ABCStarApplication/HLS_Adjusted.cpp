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

    // Use local buffer to avoid direct write-read dependencies on 'hits' array
    Hit localBuffer[MAX_HITS];
    int localHitCount = 0;

    // Decode each binary input into hits within the processHits function
    for (int i = 0; i < inputHitCount; ++i) {
        #pragma HLS PIPELINE II=1
        ap_uint<MODULE_NUMBER_BITS> moduleNumber = inputBinaries[i] >> (16 - MODULE_NUMBER_BITS);
        ap_uint<POSITION_BITS> seedPosition = (inputBinaries[i] & ((1 << POSITION_BITS) - 1));
        ap_uint<3> sizeBitmask = (inputBinaries[i] >> POSITION_BITS) & 0x7;

        // Inline decodeSize logic with direct assignment to the local buffer
        if (sizeBitmask[0] && localHitCount < MAX_HITS) localBuffer[localHitCount++] = {moduleNumber, seedPosition + 1};
        if (sizeBitmask[1] && localHitCount < MAX_HITS) localBuffer[localHitCount++] = {moduleNumber, seedPosition + 2};
        if (sizeBitmask[2] && localHitCount < MAX_HITS) localBuffer[localHitCount++] = {moduleNumber, seedPosition + 3};
    }

    outputClusterCount = 0; // Initialize final cluster count
    int currentClusterIndex = -1;

    // Iterate over the local buffer to merge hits into clusters
    for (int i = 0; i < localHitCount; ++i) {
        #pragma HLS PIPELINE II=1
        bool isNewCluster = (i == 0) || !(localBuffer[i].moduleNumber == localBuffer[i-1].moduleNumber && localBuffer[i].position == localBuffer[i-1].position + 1);
        
        if (isNewCluster) {
            currentClusterIndex++;
            if (currentClusterIndex < MAX_CLUSTERS) {
                outputClusters[currentClusterIndex] = localBuffer[i]; // Assign hit to a new cluster
            }
        } else if (currentClusterIndex < MAX_CLUSTERS - 1) {
            // If the cluster continues, but only if within bounds
            outputClusters[currentClusterIndex + 1] = localBuffer[i];
        }

        // Finalize cluster count
        if (isNewCluster) outputClusterCount++;
    }
}
