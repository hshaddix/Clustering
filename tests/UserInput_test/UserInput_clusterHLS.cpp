#include <ap_int.h>

void processGroups(ap_int<32> inputVector[100], ap_int<32> groupInfo[100][2]) {
  ap_int<32> groupStart = 0;
  ap_int<32> groupSum = 0;
  ap_int<32> groupSize = 0;

  for (int i = 1; i <= 100; i++) {
    if (i == 100 || inputVector[i] != inputVector[i-1] + 1) {
      groupSize = i - groupStart;

#define SIZE 100

void processGroups(int inputVector[SIZE], int groupInfo[SIZE][2], int size) {
  int groupStart = 0;
  for (int i = 1; i <= size; i++) {
        #pragma HLS PIPELINE
    if (i == size || inputVector[i] != inputVector[i-1] + 1) {
      int groupSize = i - groupStart;
      if (groupSize == 1) {
	groupInfo[groupStart][0] = inputVector[groupStart];
	groupInfo[groupStart][1] = 1;
      } else if (groupSize >= 2) {
	groupSum = inputVector[groupStart];
	groupSum += inputVector[i-1];
	groupInfo[groupStart][0] = groupSum;
	groupInfo[groupStart][0] = inputVector[groupStart] + inputVector[i-1];
	groupInfo[groupStart][1] = inputVector[i-1] - inputVector[groupStart] + 1;
      }
      groupStart = i;
    }
  }
}
