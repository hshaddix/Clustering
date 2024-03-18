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

    // Temporary buffer to store decoded hits and ensure each hit is processed independently
    Hit tempHitsBuffer[MAX_HITS];
    int tempHitCount = 0;

    // Decode each binary input into hits
    for (int i = 0; i < inputHitCount; ++i) {
        #pragma HLS PIPELINE II=1
        ap_uint<MODULE_NUMBER_BITS> moduleNumber = inputBinaries[i] >> (16 - MODULE_NUMBER_BITS);
        ap_uint<POSITION_BITS> seedPosition = (inputBinaries[i] & ((1 << POSITION_BITS) - 1));
        ap_uint<3> sizeBitmask = (inputBinaries[i] >> POSITION_BITS) & 0x7;

        if (sizeBitmask[0] && tempHitCount < MAX_HITS) tempHitsBuffer[tempHitCount++] = {moduleNumber, seedPosition + 1};
        if (sizeBitmask[1] && tempHitCount < MAX_HITS) tempHitsBuffer[tempHitCount++] = {moduleNumber, seedPosition + 2};
        if (sizeBitmask[2] && tempHitCount < MAX_HITS) tempHitsBuffer[tempHitCount++] = {moduleNumber, seedPosition + 3};
    }

    // Process hits from tempHitsBuffer and form clusters
    outputClusterCount = 0;
    int currentClusterIndex = -1;

    for (int i = 0; i < tempHitCount; ++i) {
        #pragma HLS PIPELINE II=1
        bool isNewCluster = (i == 0) || !(tempHitsBuffer[i].moduleNumber == tempHitsBuffer[i-1].moduleNumber && tempHitsBuffer[i].position == tempHitsBuffer[i-1].position + 1);

        if (isNewCluster) {
            currentClusterIndex++;
            if (currentClusterIndex < MAX_CLUSTERS) {
                outputClusters[currentClusterIndex] = tempHitsBuffer[i];
            }
        }

        if (isNewCluster) outputClusterCount++;
    }
}
