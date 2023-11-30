#include <iostream>
#include <vector>
#include <bitset>
#include <string>

const int STRIP_SIZE = 126;
const int HIT_ADDRESS_BITS = 8;

std::pair<int, int> calculate_if_from_sum_size(int sum, int size) {
    int f = (sum + size - 1) / 2;
    int i = (sum - size + 1) / 2;
    return {i, f};
}

bool are_adjacent(const std::pair<int, int>& cluster1, const std::pair<int, int>& cluster2) {
    return cluster1.second == cluster2.first - 1;
}

std::vector<std::pair<int, int>> merge_clusters(const std::vector<std::pair<int, int>>& clusters) {
    std::vector<std::pair<int, int>> merged;
    int offset = 0;

    for (size_t i = 0; i < clusters.size(); ++i) {
        int current_i = clusters[i].first + offset * STRIP_SIZE;
        int current_f = clusters[i].second + offset * STRIP_SIZE;

        if (!merged.empty() && are_adjacent(merged.back(), {current_i, current_f})) {
            merged.back().second = current_f;
        } else {
            merged.push_back({current_i, current_f});
        }

        // Increment offset after processing last cluster in a strip
        if (i < clusters.size() - 1 && clusters[i + 1].first == 0) {
            offset++;
        }
    }

    return merged;
}

int main() {
    std::vector<std::pair<int, int>> clusters;
    std::string binary_input_line;

    // Read binary inputs (each cluster as a line) from standard input
    while (getline(std::cin, binary_input_line)) {
        std::istringstream iss(binary_input_line);
        std::string binary_sum_str, binary_size_str;
        iss >> binary_sum_str >> binary_size_str;

        std::bitset<8> binary_sum(binary_sum_str);
        std::bitset<8> binary_size(binary_size_str);
        int sum = static_cast<int>(binary_sum.to_ulong());
        int size = static_cast<int>(binary_size.to_ulong());
        auto cluster = calculate_if_from_sum_size(sum, size);
        clusters.push_back(cluster);

        // Check if it's the last cluster in a strip
        if (binary_input_line.back() == '1') {  // Last cluster in the strip
            auto merged_clusters = merge_clusters(clusters);
            clusters.clear();

            for (const auto& cluster : merged_clusters) {
                int sum = cluster.first + cluster.second;
                int size = cluster.second - cluster.first + 1;
                std::bitset<8> binary_sum_output(sum);
                std::bitset<8> binary_size_output(size);
                std::cout << "{" << binary_sum_output.to_string() << "," << binary_size_output.to_string() << "} ";
            }
            std::cout << std::endl;
        }
    }

    return 0;
}
