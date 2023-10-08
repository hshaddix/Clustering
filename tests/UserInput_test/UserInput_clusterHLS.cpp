#include <ap_int.h>

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
	groupInfo[groupStart][0] = inputVector[groupStart] + inputVector[i-1];
	groupInfo[groupStart][1] = inputVector[i-1] - inputVector[groupStart] + 1;
      }
      groupStart = i;
    }
  }
}
