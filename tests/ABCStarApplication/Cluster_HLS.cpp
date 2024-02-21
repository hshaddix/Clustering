#include <iostream>
#include <hls_stream.h>
#include <ap_int.h> // For arbitrary precision integers

#define MAX_HITS 128 // Assuming a maximum of 128 hits for simplicity
#define MAX_CLUSTERS 32 // Assuming a maximum of 32 clusters

const int STRIP_SIZE = 126;
const int STRIP_NUMBER_BITS = 11;
const int HIT_START_SHIFT = 5;
const int POSITION_BITS = 8;
const int SIZE_BITMASK_BITS = 13;

struct Hit {
    ap_int<STRIP_NUMBER_BITS> stripNumber; // Using ap_int for HLS optimization
    ap_int<POSITION_BITS> position;
};

// Assuming a simplified version of sorting and merging due to HLS constraints
void sortHits(Hit hits[], int numHits) {
    // Simplified bubble sort for demonstration, replace with a more efficient sort for larger data sets
    bool swapped;
    do {
        swapped = false;
        for (int i = 1; i < numHits; i++) {
            if (hits[i-1].stripNumber > hits[i].stripNumber ||
                (hits[i-1].stripNumber == hits[i].stripNumber && hits[i-1].position > hits[i].position)) {
                Hit temp = hits[i-1];
                hits[i-1] = hits[i];
                hits[i] = temp;
                swapped = true;
            }
        }
    } while (swapped);
}

void decodeSize(int bitmask, int seedPosition, int stripNumber, Hit hits[], int& numHits) {
    // Reset the number of hits
    numHits = 0;
    
    // Always include the seed hit
    hits[numHits++] = {stripNumber, seedPosition};
    
    if (bitmask & 0b001) hits[numHits++] = {stripNumber, seedPosition + 3};
    if (bitmask & 0b010) hits[numHits++] = {stripNumber, seedPosition + 2};
    if (bitmask & 0b100) hits[numHits++] = {stripNumber, seedPosition + 1};
}

void mergeClusters(Hit hits[], int numHits, Hit clusters[], int& numClusters) {
    // Simplified merging logic for HLS
    numClusters = 0;
    if (numHits == 0) return;
    
    // Assuming hits are already sorted
    clusters[numClusters++] = hits[0]; // Add the first hit as the first cluster
    
    for (int i = 1; i < numHits; i++) {
        // Check if the current hit is adjacent to the last cluster
        if ((hits[i].stripNumber == clusters[numClusters-1].stripNumber && 
             hits[i].position == clusters[numClusters-1].position + 1) ||
            (hits[i].stripNumber == clusters[numClusters-1].stripNumber + 1 && 
             clusters[numClusters-1].position == STRIP_SIZE - 1 && 
             hits[i].position == 0)) {
            // Merge current hit into the last cluster
            clusters[numClusters-1] = hits[i]; // For simplicity, just update the last cluster's position
        } else {
            // Start a new cluster with the current hit
            clusters[numClusters++] = hits[i];
        }
    }
}

int main() {
    // Example usage with fixed-size arrays
    Hit hits[MAX_HITS];
    int numHits = 0;
    Hit clusters[MAX_CLUSTERS];
    int numClusters = 0;
    
    // Dummy input processing replaced by fixed input for HLS synthesis
    // Decode a fixed set of hits
    decodeSize(0b101, 50, 0, hits, numHits);
    decodeSize(0b100, 54, 0, hits, numHits);
    
    sortHits(hits, numHits); // Sort hits before merging
    mergeClusters(hits, numHits, clusters, numClusters); // Merge hits into clusters
    
    // Printing clusters is not typically done in HLS targeted code, replaced with other forms of output validation
    
    return 0;
}
