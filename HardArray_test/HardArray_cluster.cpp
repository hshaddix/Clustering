#include <iostream>
#include <vector>

void processGroups(int arr[], int size, std::vector<std::pair<int, int> >& groupInfo) {
  int groupStart = 0;
  for (int i = 1; i < size; i++) {
    if (arr[i] != arr[i-1] + 1) { //checks if current element is not one greater than the previous element
      if (i - groupStart >= 2) { // checks if group size is at least 2 
	groupInfo.push_back(std::make_pair(arr[groupStart] + arr[i-1], arr[i-1] - arr[groupStart] + 1));
      } // calculates sum and variance and pushes to vector 
      groupStart = i;
    }
  }
  if (size - groupStart >= 2) {
    groupInfo.push_back(std::make_pair(arr[groupStart] + arr[size - 1], arr[size - 1] - arr[groupStart] + 1));
  }
}

int main() {
  int inputArray[] = {11, 12, 13, 17, 18}; // Input array
  int size = sizeof(inputArray) / sizeof(inputArray[0]);
  std::vector<std::pair<int, int> > groupInfo; // Vector to store group info

  processGroups(inputArray, size, groupInfo);

  // Display results
  std::cout << "Clustered Sum/Variance: {";
  for (int i = 0; i < groupInfo.size(); i++) {
    std::cout << "{" << groupInfo[i].first << ", " << groupInfo[i].second << "}";
    if (i < groupInfo.size() - 1)
      std::cout << ", ";
  }
  std::cout << "}" << std::endl;

  return 0;
}

