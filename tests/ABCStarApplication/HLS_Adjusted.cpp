#include <ap_int.h>
#include <hls_stream.h>
#include <ap_axi_sdata.h>

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

typedef ap_axiu<16, 0, 0, 1> InputData;  // 16-bit data
typedef ap_axiu<12, 0, 0, 1> OutputData; // 12-bit data (8 bits for position + 4 bits for size)

void processHits(hls::stream<InputData> &inputBinariesStream, hls::stream<OutputData> &outputClustersStream) {
    #pragma HLS INTERFACE s_axilite port=return
    #pragma HLS INTERFACE axis port=inputBinariesStream
    #pragma HLS INTERFACE axis port=outputClustersStream

    Hit first_hit, second_hit;
    bool last = false, init = true;

    if (!inputBinariesStream.empty()) {
        first_hit = {0, 0}; // Initialize first_hit

        do {
            #pragma HLS PIPELINE
            InputData inputData = inputBinariesStream.read();
            last = inputData.last;

            ap_uint<16> inputBinary = inputData.data;
            second_hit.ABCStarID = inputBinary.range(15, 11);
            second_hit.position = inputBinary.range(10, 3);
            ap_uint<3> sizeBitmask = inputBinary.range(2, 0);

            // Update the second_hit based on the size bitmask
            if (sizeBitmask != 0) {
                if (sizeBitmask & 1) second_hit.position += 1;
                if (sizeBitmask & 2) second_hit.position += 2;
                if (sizeBitmask & 4) second_hit.position += 3;
            }

            // Check if first and second hit are adjacent
            bool isAdjacent = (second_hit.ABCStarID == first_hit.ABCStarID && second_hit.position == first_hit.position + 1) ||
                              (second_hit.ABCStarID == first_hit.ABCStarID + 1 && first_hit.position == ABCStar_SIZE - 1 && second_hit.position == 0);

            if (!isAdjacent && !init) {
                // Output the first_hit
                OutputData outputData;
                outputData.data = (first_hit.position << 4) | 1; // Example packing, assuming size is 1 for simplicity
                outputData.last = 0;
                outputClustersStream.write(outputData);
            }

            first_hit = second_hit; // Move second hit to first for next iteration
            init = false;
        } while (!last);

        // Output the last cluster
        OutputData outputData;
        outputData.data = (first_hit.position << 4) | 1; // Example packing, assuming size is 1 for simplicity
        outputData.last = 1; // This is the last output
        outputClustersStream.write(outputData);
    }
}
