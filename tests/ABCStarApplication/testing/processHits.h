// processHits.h
#include <ap_int.h>
#include <hls_stream.h>
#include <ap_axi_sdata.h>

// #define MAX_CLUSTERS 127
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
    ap_uint<POSITION_BITS> position;  // Position of the seed hit
    ap_uint<4> size;                  // Size of the cluster, up to 15
};

typedef ap_axiu<16, 0, 0, 1> InputData;  // 16-bit data

// Define a new struct for output data to encapsulate it in a stream
typedef ap_axiu<(ABCStar_ID_BITS + POSITION_BITS + SIZE_BITS), 0, 0, 1> OutputData; // Data width sum of parts

void processHits(hls::stream<InputData> &inputBinariesStream, hls::stream<OutputData> &outputClustersStream);

#endif
