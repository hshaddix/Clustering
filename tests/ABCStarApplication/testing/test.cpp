#include <ap_int.h>
#include <hls_stream.h>
#include "processHits.h"

// Function to process hits and cluster them
void processHits(hls::stream<InputData> &inputBinariesStream, int inputHitCount, ClusterInfo outputClusters[MAX_CLUSTERS], int& outputClusterCount) {
    #pragma HLS INTERFACE s_axilite port=return
    #pragma HLS INTERFACE s_axilite port=inputHitCount
    #pragma HLS INTERFACE s_axilite port=outputClusterCount
    #pragma HLS INTERFACE axis port=inputBinariesStream
    #pragma HLS INTERFACE s_axilite port=outputClusters 

    Hit hits[MAX_HITS];
    int hitCount = 0;
    outputClusterCount = 0;
    bool errorFlag = false;

    DecodeLoop: for (int i = 0; i < inputHitCount; ++i) {
        #pragma HLS PIPELINE
        InputData inputData;
        if (!inputBinariesStream.empty()) {
            inputData = inputBinariesStream.read();
        }
        
        ap_uint<16> inputBinary = inputData.data;
        // Extracting ABCStarID (first 5 bits)
        ap_uint<ABCStar_ID_BITS> ABCStarID = inputBinary.range(15, 11);
        // Extracting position (next 8 bits)
        ap_uint<POSITION_BITS> seedPosition = inputBinary.range(10, 3);
        // Extracting sizeBitmask (last 3 bits)
        ap_uint<3> sizeBitmask = inputBinary.range(2, 0);

        // Process hits based on bitmask
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

    // Post-processing: Determine cluster starts based on adjacency, including ABCStar boundaries
    int clusterIndices[MAX_HITS] = {0}; // Tracks the start index of each cluster
    int clusterSizes[MAX_HITS] = {0}; // Tracks the size of each cluster
    int numClusters = 0; // Number of clusters identified
    clusterSizes[0] = 1; // First cluster has at least one hit
    int tempClusterSize = 1; // Temporary variable to accumulate the cluster size
    
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
        
        if (!isAdjacent) {
            // Not adjacent, finalize the current cluster's size
            clusterSizes[numClusters] = tempClusterSize;
            numClusters++; // Start a new cluster
            clusterIndices[numClusters] = i; // Mark the new cluster's starting index
            tempClusterSize = 1; // Reset for the new cluster
        } else {
            // Increment the size for the ongoing cluster
            tempClusterSize++;
        }
    }
    // Update the size of the last cluster after loop completion
    clusterSizes[numClusters] = tempClusterSize;
    
    // Safely update outputClusters based on identified clusters
    for (int i = 0; i <= numClusters; ++i) {
        #pragma HLS PIPELINE
        if (i < MAX_CLUSTERS) {
            outputClusters[i].firstHit = hits[clusterIndices[i]]; // Assign the first hit of each cluster
            outputClusters[i].size = clusterSizes[i]; // Assign the calculated size of each cluster
        } else {
            errorFlag = true; // Set the error flag if exceeding MAX_CLUSTERS
            break; // Optionally break out of the loop to stop processing further clusters
        }
    }
    outputClusterCount = numClusters + 1; // Update the total number of clusters identified

    // Check if an error occurred due to exceeding MAX_CLUSTERS
    if (errorFlag) {
        outputClusterCount = MAX_CLUSTERS;
    }
}
