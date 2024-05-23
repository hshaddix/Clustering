#include <ap_int.h>
#include <hls_stream.h>
#include <ap_axi_sdata.h>
#include "processHits.h"

struct OutputClusterInfo {
    ap_uint<ABCStar_ID_BITS> ABCStarID; // ABCStarID of the cluster
    ap_uint<POSITION_BITS> position;    // Position of the seed hit
    ap_uint<SIZE_BITS> size;            // Size of the cluster, up to 7 hits
};

// Define a new struct for input data to encapsulate it in a stream
typedef ap_axiu<16, 0, 0, 1> InputData;  // 16-bit data

// Define a new struct for output data to encapsulate it in a stream
typedef ap_axiu<(ABCStar_ID_BITS + POSITION_BITS + SIZE_BITS), 0, 0, 1> OutputData; // Data width sum of parts

// Outputs a cluster to the stream
void outputCluster(const Hit &hit, int size, hls::stream<OutputData> &stream, bool isLast = false) {
    OutputData outputData;
    outputData.data = (hit.ABCStarID << (POSITION_BITS + SIZE_BITS)) | (hit.position << SIZE_BITS) | size;
    outputData.last = isLast ? 1 : 0;
    stream.write(outputData);
    std::cout << "Output cluster: ABCStarID = " << hit.ABCStarID << ", Position = " << hit.position << ", Size = " << size << ", Last = " << outputData.last << std::endl;
}

// Function to process hits and cluster them
void processHits(hls::stream<InputData> &inputBinariesStream, hls::stream<OutputData> &outputClustersStream) {
    #pragma HLS INTERFACE s_axilite port=return
    #pragma HLS INTERFACE axis port=inputBinariesStream
    #pragma HLS INTERFACE axis port=outputClustersStream

    Hit current_hit;
    bool last = false;

    while (!inputBinariesStream.empty()) {
        InputData inputData = inputBinariesStream.read();
        last = inputData.last;

        current_hit.ABCStarID = inputData.data.range(15, 11);
        int basePosition = inputData.data.range(10, 3);
        ap_uint<3> sizeBitmask = inputData.data.range(2, 0);

        std::cout << "Read Data: ABCStarID = " << current_hit.ABCStarID << ", Base Position = " << basePosition << ", Bitmask = " << sizeBitmask << std::endl;

        switch (sizeBitmask) {
            case 0: // 000
                current_hit.position = basePosition;
                outputCluster(current_hit, 1, outputClustersStream, last);
                break;

            case 1: // 001
                current_hit.position = basePosition;
                outputCluster(current_hit, 1, outputClustersStream);
                current_hit.position = basePosition + 1;
                outputCluster(current_hit, 1, outputClustersStream, last);
                break;

            case 2: // 010
                current_hit.position = basePosition;
                outputCluster(current_hit, 1, outputClustersStream);
                current_hit.position = basePosition + 2;
                outputCluster(current_hit, 1, outputClustersStream, last);
                break;

            case 3: // 011
                current_hit.position = basePosition;
                outputCluster(current_hit, 1, outputClustersStream);
                current_hit.position = basePosition + 2;
                outputCluster(current_hit, 2, outputClustersStream, last);
                break;

            case 4: // 100
                current_hit.position = basePosition;
                outputCluster(current_hit, 1, outputClustersStream);
                current_hit.position = basePosition + 3;
                outputCluster(current_hit, 1, outputClustersStream, last);
                break;

            case 5: // 101
                current_hit.position = basePosition;
                outputCluster(current_hit, 1, outputClustersStream);
                current_hit.position = basePosition + 1;
                outputCluster(current_hit, 2, outputClustersStream);
                current_hit.position = basePosition + 3;
                outputCluster(current_hit, 1, outputClustersStream, last);
                break;

            case 6: // 110
                current_hit.position = basePosition;
                outputCluster(current_hit, 1, outputClustersStream);
                current_hit.position = basePosition + 1;
                outputCluster(current_hit, 2, outputClustersStream);
                current_hit.position = basePosition + 2;
                outputCluster(current_hit, 1, outputClustersStream, last);
                break;

            case 7: // 111
                current_hit.position = basePosition;
                outputCluster(current_hit, 1, outputClustersStream);
                current_hit.position = basePosition + 1;
                outputCluster(current_hit, 2, outputClustersStream);
                current_hit.position = basePosition + 2;
                outputCluster(current_hit, 1, outputClustersStream);
                current_hit.position = basePosition + 3;
                outputCluster(current_hit, 1, outputClustersStream, last);
                break;

            default:
                break;
        }
    }
}
