#include <ap_int.h>

#define SIZE 100

void processGroups(int inputVector[SIZE], int groupInfo[SIZE][2], int size) {
    #pragma HLS PIPELINE II=2

  int groupStart = 0;
  int prevValue = inputVector[0];

  for (int i = 1; i < size; i++) {
    int currentValue = inputVector[i];

    if (currentValue != prevValue + 1) {
      int groupSize = i - groupStart;

      if (groupSize == 1) {
	groupInfo[groupStart][0] = prevValue;
	groupInfo[groupStart][1] = 1;
      } else if (groupSize >= 2) {
	groupInfo[groupStart][0] = prevValue + currentValue;
	groupInfo[groupStart][1] = currentValue - prevValue + 1;
      }

      groupStart = i;
      prevValue = currentValue;
    }
  }
}
