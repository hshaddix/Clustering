// processHits.h
#ifndef PROCESS_HITS_H
#define PROCESS_HITS_H

#include <ap_int.h>
#include <hls_stream.h>

#define MAX_HITS 1024
#define MAX_CLUSTERS 127
#define ABCStar_ID_BITS 5
#define POSITION_BITS 8
#define ABCStar_SIZE 128

struct Hit {
    ap_uint<ABCStar_ID_BITS> ABCStarID;
    ap_uint<POSITION_BITS> position;
};

struct ClusterInfo {
    Hit firstHit;
    int size;
};

struct InputData {
    ap_uint<16> data;
};

void processHits(hls::stream<InputData>& inputBinariesStream, int inputHitCount, ClusterInfo outputClusters[MAX_CLUSTERS], int& outputClusterCount);

#endif
