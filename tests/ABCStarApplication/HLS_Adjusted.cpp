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

void processHits(ap_uint<16> inputBinaries[MAX_HITS], int inputHitCount, Hit outputClusters[MAX_CLUSTERS], int& outputClusterCount) {
    #pragma HLS INTERFACE s_axilite port=return
    #pragma HLS INTERFACE s_axilite port=inputHitCount
    #pragma HLS INTERFACE s_axilite port=outputClusterCount
    #pragma HLS INTERFACE m_axi depth=MAX_HITS port=inputBinaries
    #pragma HLS INTERFACE m_axi depth=MAX_CLUSTERS port=outputClusters

    outputClusterCount = 0;

    ap_uint<MODULE_NUMBER_BITS> lastModuleNumber = 0;
    ap_uint<POSITION_BITS> lastPosition = 0;
    bool firstHit = true;

    for (int i = 0; i < inputHitCount && outputClusterCount < MAX_CLUSTERS; ++i) {
        #pragma HLS PIPELINE II=1
        ap_uint<16> inputBinary = inputBinaries[i];
        ap_uint<MODULE_NUMBER_BITS> moduleNumber = inputBinary >> (16 - MODULE_NUMBER_BITS);
        ap_uint<POSITION_BITS> seedPosition = (inputBinary & ((1 << POSITION_BITS) - 1));
        ap_uint<3> sizeBitmask = (inputBinary >> POSITION_BITS) & 0x7;

        for (ap_uint<2> j = 0; j < 3; j++) {
            if (sizeBitmask[j]) {
                ap_uint<POSITION_BITS> position = seedPosition + j + 1; // Correctly accounting for cluster size
                bool isNewCluster = firstHit || !(moduleNumber == lastModuleNumber && position == lastPosition + 1);

                if (isNewCluster) {
                    if (!firstHit) outputClusterCount++;
                    if (outputClusterCount < MAX_CLUSTERS) {
                        outputClusters[outputClusterCount].moduleNumber = moduleNumber;
                        outputClusters[outputClusterCount].position = position;
                    }
                    firstHit = false;
                }

                lastModuleNumber = moduleNumber;
                lastPosition = position;
            }
        }
    }

    if (!firstHit) outputClusterCount++; // Account for the last cluster
}
