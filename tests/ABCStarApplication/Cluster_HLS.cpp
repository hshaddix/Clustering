#include <ap_int.h>
#include <hls_stream.h>

#define MAX_HITS 1024
#define STRIP_SIZE 126
#define STRIP_NUMBER_BITS 11
#define POSITION_BITS 8
#define SIZE_BITMASK_BITS 3 // Assuming 3 bits for size bitmask as per your logic

struct Hit {
    ap_uint<STRIP_NUMBER_BITS> stripNumber;
    ap_uint<POSITION_BITS> position;
};

// Function prototypes
void processHits(ap_uint<16> inputBinaries[MAX_HITS], int inputHitCount, Hit outputClusters[MAX_HITS], int& outputClusterCount);
void sortHits(Hit hits[MAX_HITS], int n);
int mergeClusters(Hit hits[MAX_HITS], int hitCount, Hit clusters[MAX_HITS]);

// Decode the size bitmask and populate hits array
void decodeSize(ap_uint<SIZE_BITMASK_BITS> bitmask, int seedPosition, int stripNumber, Hit hits[MAX_HITS], int& hitCount) {
    #pragma HLS INLINE
    if (bitmask[0]) hits[hitCount++] = {stripNumber, seedPosition + 1};
    if (bitmask[1]) hits[hitCount++] = {stripNumber, seedPosition + 2};
    if (bitmask[2]) hits[hitCount++] = {stripNumber, seedPosition + 3};
}

// Sort hits by strip number and position using a simple bubble sort algorithm
void sortHits(Hit hits[MAX_HITS], int n) {
    #pragma HLS INLINE
    Hit temp;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (hits[j].stripNumber > hits[j + 1].stripNumber || 
               (hits[j].stripNumber == hits[j + 1].stripNumber && hits[j].position > hits[j + 1].position)) {
                temp = hits[j];
                hits[j] = hits[j + 1];
                hits[j + 1] = temp;
            }
        }
    }
}

// Merge adjacent hits into clusters
int mergeClusters(Hit hits[MAX_HITS], int hitCount, Hit clusters[MAX_HITS]) {
    #pragma HLS INLINE
    int clusterCount = 0;
    if (hitCount == 0) return 0;

    clusters[0] = hits[0]; // Initialize the first cluster
    for (int i = 1; i < hitCount; i++) {
        #pragma HLS LOOP_FLATTEN off
        #pragma HLS PIPELINE II=1
        if (hits[i].stripNumber != hits[i - 1].stripNumber || hits[i].position != hits[i - 1].position + 1) {
            // New cluster
            clusterCount++;
        }
        clusters[clusterCount] = hits[i];
    }
    return clusterCount + 1; // Include the last cluster
}

// Top-level function for HLS synthesis
void processHits(ap_uint<16> inputBinaries[MAX_HITS], int inputHitCount, Hit outputClusters[MAX_HITS], int& outputClusterCount) {
    #pragma HLS INTERFACE s_axilite port=return
    #pragma HLS INTERFACE s_axilite port=inputHitCount
    #pragma HLS INTERFACE s_axilite port=outputClusterCount
    #pragma HLS INTERFACE m_axi depth=MAX_HITS port=inputBinaries
    #pragma HLS INTERFACE m_axi depth=MAX_HITS port=outputClusters

    Hit hits[MAX_HITS];
    int hitCount = 0;
    #pragma HLS ARRAY_PARTITION variable=hits complete dim=1

    // Parse, decode, and sort hits
    for (int i = 0; i < inputHitCount; i++) {
        #pragma HLS PIPELINE II=1
        ap_uint<STRIP_NUMBER_BITS> stripNumber = inputBinaries[i] >> POSITION_BITS;
        ap_uint<POSITION_BITS> seedPosition = inputBinaries[i] & ((1 << POSITION_BITS) - 1);
        ap_uint<SIZE_BITMASK_BITS> sizeBitmask = inputBinaries[i] >> (16 - SIZE_BITMASK_BITS);
        decodeSize(sizeBitmask, seedPosition, stripNumber, hits, hitCount);
    }

    sortHits(hits, hitCount);

    // Merge hits into clusters and output
    outputClusterCount = mergeClusters(hits, hitCount, outputClusters);
}

