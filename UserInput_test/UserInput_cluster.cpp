#include <iostream>
#include <vector>

void processGroups(std::vector<int>& inputVector, std::vector<std::pair<int, int> >& groupInfo) {
  int groupStart = 0;
  for (size_t i = 1; i < inputVector.size(); i++) {
    if (inputVector[i] != inputVector[i-1] + 1) {
      if (i - groupStart >= 2) {
        groupInfo.push_back(std::make_pair(inputVector[groupStart] + inputVector[i-1], inputVector[i-1] - inputVector[groupStart] + 1));
      }
      groupStart = i;
    }
  }
  if (inputVector.size() - groupStart >= 2) {
    groupInfo.push_back(std::make_pair(inputVector[groupStart] + inputVector[inputVector.size() - 1], inputVector[inputVector.size() - 1] - inputVector[groupStart] + 1));
  }
}

int main() {
  std::vector<int> inputVector; // Input vector
  int num;
  
  // Get user input
  std::cout << "Enter numbers (enter a non-integer to stop): ";
  while (std::cin >> num) {
    inputVector.push_back(num);
  }

  std::vector<std::pair<int, int> > groupInfo; // Vector to store group info

  processGroups(inputVector, groupInfo);

  // Display results
  std::cout << "Clustered Sum/Variance: {";
  for (size_t i = 0; i < groupInfo.size(); i++) {
    std::cout << "{" << groupInfo[i].first << ", " << groupInfo[i].second << "}";
    if (i < groupInfo.size() - 1)
      std::cout << ", ";
  }
  std::cout << "}" << std::endl;

  return 0;
}
