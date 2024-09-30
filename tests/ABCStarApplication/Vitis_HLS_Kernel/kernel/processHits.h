#ifndef PROCESS_HITS_H
#define PROCESS_HITS_H

#include <ap_int.h>
#include <hls_stream.h>
#include <ap_axi_sdata.h>

#define POSITION_BITS 13
#define SIZE_BITS 3
#define ABCStar_SIZE 256

#define INPUTDATASIZE 4096

struct Hit {
    ap_uint<POSITION_BITS> position;
    ap_uint<SIZE_BITS> size;
};

// typedef ap_axiu<16, 0, 0, 1> InputData;  // 16-bit data
// typedef ap_axiu<(POSITION_BITS + SIZE_BITS), 0, 0, 1> OutputData; // Data width sum of parts

typedef ap_uint<16> InputData;
typedef Hit OutputData;

extern "C"
{
    // void processHits(hls::stream<InputData> &inputBinariesStream, hls::stream<OutputData> &outputClustersStream);
    void processHits(InputData* in1_gmem, OutputData* out1_gmem);
}
#endif // PROCESS_HITS_H
