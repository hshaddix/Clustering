#include <ap_int.h>
#include <hls_stream.h>
#include <ap_axi_sdata.h>

#define MAX_HITS 1024
#define MAX_CLUSTERS 127
#define ABCStar_ID_BITS 5
#define POSITION_BITS 8
#define ABCStar_SIZE 128 // Assuming a fixed ABCStar size of 128 positions.

struct Hit {
    ap_uint<ABCStar_ID_BITS> ABCStarID;
    ap_uint<POSITION_BITS> position;
};

struct OutputClusterInfo {
    ap_uint<POSITION_BITS> position;  // Position of the seed hit
    ap_uint<4> size;                  // Size of the cluster, up to 15
};

// Define a new struct for input data to encapsulate it in a stream
typedef ap_axiu<16, 0, 0, 1> InputData;  // 16-bit data

// Define a new struct for output data to encapsulate it in a stream
typedef ap_axiu<12, 0, 0, 1> OutputData; // 12-bit data (8 bits for position + 4 bits for size)

// Function to process hits and cluster them
void processHits(hls::stream<InputData> &inputBinariesStream, hls::stream<OutputData> &outputClustersStream) {
    #pragma HLS INTERFACE s_axilite port=return
    #pragma HLS INTERFACE axis port=inputBinariesStream
    #pragma HLS INTERFACE axis port=outputClustersStream

    #pragma HLS ARRAY_PARTITION variable=hits complete
    Hit hits[MAX_HITS];
    int hitCount = 0;

    InputData inputData;
    bool last = false;
    do {
        #pragma HLS PIPELINE
        inputData = inputBinariesStream.read();
        last = inputData.last;

        ap_uint<16> inputBinary = inputData.data;
        ap_uint<ABCStar_ID_BITS> ABCStarID = inputBinary.range(15, 11);
        ap_uint<POSITION_BITS> seedPosition = inputBinary.range(10, 3);
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
    } while (!last);

    // Post-processing: Determine cluster starts based on adjacency, including ABCStar boundaries
    int clusterStartIndex = 0;
    for (int i = 1; i < hitCount; i++) {
        #pragma HLS PIPELINE
        // Check adjacency within the same ABCStar or across boundaries
        bool isAdjacent = (hits[i].ABCStarID == hits[i-1].ABCStarID && hits[i].position == hits[i-1].position + 1) ||
                          (hits[i].ABCStarID == hits[i-1].ABCStarID + 1 && hits[i-1].position == ABCStar_SIZE - 1 && hits[i].position == 0);

        if (!isAdjacent) {
            // Output the cluster formed before this hit
            OutputData outputData;
            outputData.data = (hits[clusterStartIndex].position << 4) | (i - clusterStartIndex); // pack position and size
            outputData.last = 0; // Not the last output unless set below
            outputClustersStream.write(outputData);
            clusterStartIndex = i;
        }
    }

    // Output the final cluster
    OutputData outputData;
    outputData.data = (hits[clusterStartIndex].position << 4) | (hitCount - clusterStartIndex); // pack position and size
    outputData.last = 1; // Mark this as the last cluster output
    outputClustersStream.write(outputData);
}

