#include <iostream>
#include <vector>
#include <bitset>
#include <string>

const int STRIP_SIZE = 126;

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
    bool last_in_strip = false;

    for (const auto& cluster : clusters) {
        int current_i = cluster.first + offset * STRIP_SIZE;
        int current_f = cluster.second + offset * STRIP_SIZE;

        if (last_in_strip && are_adjacent(merged.back(), {current_i, current_f})) {
            merged.back().second = current_f;
        } else {
            merged.push_back({current_i, current_f});
        }

        last_in_strip = (cluster.first == 0 && cluster.second == 0); // Empty cluster indicates last in strip
        if (last_in_strip) {
            offset++;
        }
    }

    return merged;
}

int main() {
    std::vector<std::pair<int, int>> clusters;
    std::string binary_input;
    std::cin >> binary_input;

    for (size_t i = 0; i < binary_input.length(); i += 16) {  // Each cluster is 16 bits
        std::bitset<8> binary_sum(binary_input.substr(i, 8));
        std::bitset<8> binary_size(binary_input.substr(i + 8, 8));
        clusters.push_back(calculate_if_from_sum_size(binary_sum.to_ulong(), binary_size.to_ulong()));
    }

    auto merged_clusters = merge_clusters(clusters);

    for (const auto& cluster : merged_clusters) {
        int sum = cluster.first + cluster.second;
        int size = cluster.second - cluster.first + 1;
        std::bitset<8> binary_sum_output(sum);
        std::bitset<8> binary_size_output(size);
        std::cout << "{" << binary_sum_output.to_string() << "," << binary_size_output.to_string() << "} ";
    }
    std::cout << std::endl;

    return 0;
}
