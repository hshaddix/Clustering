#include <ap_int.h>
#include <hls_stream.h>
#include <ap_axi_sdata.h>

#define MAX_HITS 1024
#define ABCStar_ID_BITS 5
#define POSITION_BITS 8
#define ABCStar_SIZE 128 // Assuming a fixed ABCStar size of 128 positions.

struct Hit {
    ap_uint<ABCStar_ID_BITS> ABCStarID;
    ap_uint<POSITION_BITS> position;
};

struct OutputClusterInfo {
    ap_uint<POSITION_BITS> position;  // Position of the seed hit
    ap_uint<4> size;                  // Size of the cluster, up to 15 hits
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

    Hit first_hit, second_hit;
    bool init = true, last = false;
    int clusterSize = 0;

    do {
        #pragma HLS PIPELINE
        InputData inputData = inputBinariesStream.read();
        last = inputData.last;
        ap_uint<16> inputBinary = inputData.data;

        if (!init) {
            first_hit = second_hit;  // Prepare the first_hit for comparison
        }

        second_hit.ABCStarID = inputBinary.range(15, 11);
        second_hit.position = inputBinary.range(10, 3);
        ap_uint<3> sizeBitmask = inputBinary.range(2, 0);

        // Decode bitmask to adjust the position of second_hit accordingly
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

        // Check if the first and second hits are adjacent
        bool isAdjacent = (first_hit.ABCStarID == second_hit.ABCStarID && second_hit.position == first_hit.position + 1) ||
                          (first_hit.ABCStarID + 1 == second_hit.ABCStarID && first_hit.position == ABCStar_SIZE - 1 && second_hit.position == 0);

        if (!isAdjacent && !init) {
            // Output the cluster information
            OutputData outputData;
            outputData.data = (first_hit.position << 4) | clusterSize; // Pack position and size
            outputData.last = 0;
            outputClustersStream.write(outputData);

            // Reset for the next cluster
            clusterSize = 1;
        } else {
            // Increment the cluster size if adjacent or initialize if it's the start
            clusterSize++;
        }

        init = false;
    } while (!last);

    // Output the last cluster
    OutputData outputData;
    outputData.data = (second_hit.position << 4) | clusterSize; // Pack position and size
    outputData.last = 1; // Mark this as the last cluster output
    outputClustersStream.write(outputData);
}
