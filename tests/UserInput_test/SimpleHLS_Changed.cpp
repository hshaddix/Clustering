#include <ap_int.h>

#define SIZE 100

void processGroups(int inputVector[SIZE], int groupInfo[SIZE][2], int size) {
    #pragma HLS PIPELINE II=2
    #pragma HLS LOOP_TRIPCOUNT min=1 max=100

  if (size == 1) {
    groupInfo[0][0] = inputVector[0];
    groupInfo[0][1] = 1;
    return;
  }

  int groupStart = 0;
  int prevValue = inputVector[0];

  for (int i = 1; i < size; i+=2) {
    int currentValue1 = inputVector[i];
    int currentValue2 = (i+1 < size) ? inputVector[i+1] : currentValue1;

    if (currentValue1 != prevValue + 1) {
      int groupSize = i - groupStart;

      if (groupSize == 1) {
	groupInfo[groupStart][0] = prevValue;
	groupInfo[groupStart][1] = 1;
      } else if (groupSize >= 2) {
	groupInfo[groupStart][0] = prevValue + currentValue1;
	groupInfo[groupStart][1] = currentValue1 - prevValue + 1;
      }

      groupStart = i;
      prevValue = currentValue1;
    }

    if (currentValue2 != currentValue1 + 1) {
      int groupSize = i+1 - groupStart;

      if (groupSize == 1) {
	groupInfo[groupStart][0] = currentValue1;
	groupInfo[groupStart][1] = 1;
      } else if (groupSize >= 2) {
	groupInfo[groupStart][0] = currentValue1 + currentValue2;
	groupInfo[groupStart][1] = currentValue2 - currentValue1 + 1;
      }

      groupStart = i+1;
      prevValue = currentValue2;
    }
  }
}
