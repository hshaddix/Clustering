#include <ap_int.h>
#include <hls_stream.h>
#include <ap_axi_sdata.h>

#define ABCStar_ID_BITS 5
#define POSITION_BITS 8
#define SIZE_BITS 3
#define ABCStar_SIZE 128 // Assuming a fixed ABCStar size of 128 positions.

struct Hit {
    ap_uint<ABCStar_ID_BITS> ABCStarID;
    ap_uint<POSITION_BITS> position;
};

struct OutputClusterInfo {
    ap_uint<ABCStar_ID_BITS> ABCStarID; // ABCStarID of the cluster
    ap_uint<POSITION_BITS> position;   // Position of the seed hit
    ap_uint<SIZE_BITS> size;           // Size of the cluster, up to 7 hits
};

// Define a new struct for input data to encapsulate it in a stream
typedef ap_axiu<16, 0, 0, 1> InputData;  // 16-bit data

// Define a new struct for output data to encapsulate it in a stream
typedef ap_axiu<(ABCStar_ID_BITS + POSITION_BITS + SIZE_BITS), 0, 0, 1> OutputData; // Data width sum of parts

// Function to process hits and cluster them
void processHits(hls::stream<InputData> &inputBinariesStream, hls::stream<OutputData> &outputClustersStream) {
    #pragma HLS INTERFACE s_axilite port=return
    #pragma HLS INTERFACE axis port=inputBinariesStream
    #pragma HLS INTERFACE axis port=outputClustersStream

    Hit first_hit, second_hit;
    bool init = true, last = false;
    int clusterSize = 0;

    while (!inputBinariesStream.empty()) {  // Ensure there is data before starting to read
        InputData inputData = inputBinariesStream.read();
        last = inputData.last;

        if (!init) {
            first_hit = second_hit;  // Prepare the first_hit for comparison
        }

        // Decode the input data
        second_hit.ABCStarID = inputData.data.range(15, 11);
        second_hit.position = inputData.data.range(10, 3);
        ap_uint<3> sizeBitmask = inputData.data.range(2, 0);

        // Decode bitmask and adjust the position of second_hit accordingly
        switch(sizeBitmask.to_uint()) {
            case 1: // 001
                second_hit.position += 3;
                break;
            case 2: // 010
                second_hit.position += 2;
                break;
            case 3: // 011
                second_hit.position += 2;
                second_hit.position += 3;
                break;
            case 4: // 100
                second_hit.position += 1;
                break;
            case 5: // 101
                second_hit.position += 1;
                second_hit.position += 3;
                break;
            case 6: // 110
                second_hit.position += 1;
                second_hit.position += 2;
                break;
            case 7: // 111
                second_hit.position += 1;
                second_hit.position += 2;
                second_hit.position += 3;
                break;
            default:
                // No action on default
                break;
        }

        // Check adjacency
        bool isAdjacent = (first_hit.ABCStarID == second_hit.ABCStarID && second_hit.position == first_hit.position + 1) ||
                          (first_hit.ABCStarID + 1 == second_hit.ABCStarID && first_hit.position == ABCStar_SIZE - 1 && second_hit.position == 0);

        if (!isAdjacent && !init) {
            // Output the cluster
            OutputData outputData;
            outputData.data = (first_hit.ABCStarID << (POSITION_BITS + SIZE_BITS)) | (first_hit.position << SIZE_BITS) | clusterSize;
            outputData.last = 0;
            outputClustersStream.write(outputData);
            clusterSize = 1;
        } else {
            clusterSize++;
        }

        init = false;
        if (last) {
            OutputData outputData;
            outputData.data = (second_hit.ABCStarID << (POSITION_BITS + SIZE_BITS)) | (second_hit.position << SIZE_BITS) | clusterSize;
            outputData.last = 1;
            outputClustersStream.write(outputData);
        }
    }
}
