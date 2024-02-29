#include <ap_int.h>
#include <hls_stream.h>

#define MAX_HITS 1024
#define MAX_CLUSTERS 128
#define STRIP_SIZE 126
#define STRIP_NUMBER_BITS 11
#define HIT_START_SHIFT 5
#define POSITION_BITS 8
#define SIZE_BITMASK_BITS 13

struct Hit {
    ap_uint<STRIP_NUMBER_BITS> stripNumber;
    ap_uint<POSITION_BITS> position;
};

// Function prototypes
void sortHits(Hit hits[], int n);
int decodeSize(ap_uint<16> bitmask, int &hitCount, Hit hits[], int stripNumber, int seedPosition);
int parseCluster(const ap_uint<16> &binary, Hit hits[], int hitCount);
int mergeClusters(Hit hits[], int hitCount, Hit clusters[MAX_CLUSTERS][MAX_HITS], int clusterSizes[MAX_CLUSTERS]);

// HLS interface pragmas adjustments for processHits function
void processHits(ap_uint<16> inputBinaries[MAX_HITS], int inputHitCount, Hit outputClusters[MAX_CLUSTERS][MAX_HITS], int& outputClusterCount) {
    #pragma HLS INTERFACE s_axilite port=return
    #pragma HLS INTERFACE s_axilite port=inputHitCount
    #pragma HLS INTERFACE s_axilite port=outputClusterCount
    #pragma HLS INTERFACE m_axi depth=MAX_HITS port=inputBinaries
    #pragma HLS INTERFACE m_axi depth=MAX_CLUSTERS*MAX_HITS port=outputClusters

    Hit hits[MAX_HITS];
    int clusterSizes[MAX_CLUSTERS] = {0};
    int hitCount = 0;

    // Adjustments for loop pipelining and dataflow optimization
    #pragma HLS DATAFLOW

    for (int i = 0; i < inputHitCount; ++i) {
        #pragma HLS PIPELINE II=1
        hitCount = parseCluster(inputBinaries[i], hits, hitCount);
        if(hitCount >= MAX_HITS) break;
    }

    sortHits(hits, hitCount); // Note: Consider loop unrolling or pipelining optimizations here
    outputClusterCount = mergeClusters(hits, hitCount, outputClusters, clusterSizes);
}

// Main function remains the same, primarily for simulation
int main() {
    // Simulation code here
    return 0;
}
