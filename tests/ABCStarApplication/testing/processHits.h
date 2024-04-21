// processHits.h
#ifndef PROCESS_HITS_H
#define PROCESS_HITS_H

#include <ap_int.h>
#include <hls_stream.h>
#include <ap_axi_sdata.h>

#define ABCStar_ID_BITS 5
#define POSITION_BITS 8
#define SIZE_BITS 3
#define ABCStar_SIZE 128

struct Hit {
    ap_uint<ABCStar_ID_BITS> ABCStarID;
    ap_uint<POSITION_BITS> position;
};

typedef ap_axiu<16, 0, 0, 1> InputData;  // 16-bit data
typedef ap_axiu<(ABCStar_ID_BITS + POSITION_BITS + SIZE_BITS), 0, 0, 1> OutputData; // Data width sum of parts

void processHits(hls::stream<InputData> &inputBinariesStream, hls::stream<OutputData> &outputClustersStream);

#endif // PROCESS_HITS_H
