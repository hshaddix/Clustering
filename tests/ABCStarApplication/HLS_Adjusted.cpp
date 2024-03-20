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
    #pragma HLS DATAFLOW

    hls::stream<Hit> hitStream;
    #pragma HLS STREAM variable=hitStream depth=256

    // Decode and stream hits
    for (int i = 0; i < inputHitCount; ++i) {
        #pragma HLS PIPELINE II=1
        ap_uint<MODULE_NUMBER_BITS> moduleNumber = inputBinaries[i] >> (16 - MODULE_NUMBER_BITS);
        ap_uint<POSITION_BITS> seedPosition = (inputBinaries[i] & ((1 << POSITION_BITS) - 1));
        ap_uint<3> sizeBitmask = (inputBinaries[i] >> POSITION_BITS) & 0x7;

        if (sizeBitmask[0]) hitStream.write({moduleNumber, seedPosition + 1});
        if (sizeBitmask[1]) hitStream.write({moduleNumber, seedPosition + 2});
        if (sizeBitmask[2]) hitStream.write({moduleNumber, seedPosition + 3});
    }

    outputClusterCount = 0;

    // Process streamed hits to form clusters
    Hit previousHit = {0, 0};
    for (int i = 0; i < MAX_HITS && !hitStream.empty(); ++i) {
        #pragma HLS PIPELINE II=1
        Hit currentHit;
        hitStream.read(currentHit);
        bool isNewCluster = (i == 0) || !(currentHit.moduleNumber == previousHit.moduleNumber && currentHit.position == previousHit.position + 1);

        if (isNewCluster) {
            if (outputClusterCount < MAX_CLUSTERS) {
                outputClusters[outputClusterCount++] = currentHit;
            }
        }
        previousHit = currentHit;
    }
}
