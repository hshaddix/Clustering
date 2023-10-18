#include <iostream>
#include <vector>

const int STRIP_SIZE = 126;

void processStrips(std::vector<int>& inputVector, std::vector<std::pair<int, int> >& stripInfo) {
  int stripStart = 0;
  for (size_t i = 1; i <= inputVector.size(); i++) {
    if (i % STRIP_SIZE == 0 || i == inputVector.size() || inputVector[i] != inputVector[i-1] + 1) {
      int stripSize = i - stripStart;
      if (stripSize == 1) {
        stripInfo.push_back(std::make_pair(inputVector[stripStart], 1));
      } else if (stripSize >= 2) {
        stripInfo.push_back(std::make_pair(inputVector[stripStart] + inputVector[i-1], inputVector[i-1] - inputVector[stripStart] + 1));
      }
      stripStart = i;
    }
  }

  // Merge adjacent strips at boundary
  for (size_t i = 1; i < stripInfo.size(); i++) {
    if (stripInfo[i].first == stripInfo[i-1].first + stripInfo[i-1].second) {
      stripInfo[i].first = stripInfo[i-1].first;
      stripInfo[i].second += stripInfo[i-1].second;
      stripInfo.erase(stripInfo.begin() + i - 1);
      i--;
    }
  }
}

int main() {
  std::vector<int> inputVector; // Input vector
  int num;

  // Get user input
  std::cout << "Enter numbers (enter a non-integer to stop): ";
  while (std::cin >> num && num >= 0 && num <= 100) {
    inputVector.push_back(num);
  }

  std::vector<std::pair<int, int> > stripInfo; // Vector to store strip info

  processStrips(inputVector, stripInfo);

  // Display results
  std::cout << "Clustered Sum/Variance: {";
  for (size_t i = 0; i < stripInfo.size(); i++) {
    std::cout << "{" << stripInfo[i].first << ", " << stripInfo[i].second << "}";
    if (i < stripInfo.size() - 1)
      std::cout << ", ";
  }
  std::cout << "}" << std::endl;

  return 0;
}
