#include <iostream>
#include <vector>
#include <bitset>
#include <string>

const int STRIP_SIZE = 126;

std::pair<int, int> calculate_if_from_sum_size(int sum, int size) {
    if (sum == 0 && size == 0) {
        return {-1, -1}; // Indicator for an empty strip
    }
    int f = (sum + size - 1) / 2;
    int i = (sum - size + 1) / 2;
    return {i, f};
}

bool are_adjacent(int f1, int i2) {
    return f1 + 1 == i2;
}

std::vector<std::pair<int, int>> merge_clusters(const std::vector<std::pair<int, int>>& clusters) {
    std::vector<std::pair<int, int>> merged;
    int offset = 0;

    for (const auto& cluster : clusters) {
        if (cluster.first == -1) {
            offset++;
            continue; // Skip the empty strip and increase offset
        }

        int current_i = cluster.first + offset * STRIP_SIZE;
        int current_f = cluster.second + offset * STRIP_SIZE;

        if (!merged.empty() && are_adjacent(merged.back().second, current_i)) {
            // Merge with the previous cluster
            merged.back().second = current_f;
        } else {
            merged.push_back({current_i, current_f});
        }
    }

    return merged;
}

std::string convert_to_binary(int num) {
    std::bitset<8> binary(num);
    return binary.to_string();
}

int main() {
    std::vector<std::pair<int, int>> clusters;
    std::string binary_input;

    while (std::cin >> binary_input) {
        std::bitset<8> binary_sum(binary_input.substr(0, 8));
        std::bitset<8> binary_size(binary_input.substr(8, 8));
        auto cluster = calculate_if_from_sum_size(binary_sum.to_ulong(), binary_size.to_ulong());

        if (cluster.first == -1) {
            std::cout << "FE ";
            continue;
        }

        clusters.push_back(cluster);
    }

    auto merged_clusters = merge_clusters(clusters);

    for (const auto& cluster : merged_clusters) {
        if (cluster.first == -1) {
            continue; // Skip processing for empty strip
        }

        std::cout << "{" << convert_to_binary(cluster.first) << "," << convert_to_binary(cluster.second) << "} ";
    }
    std::cout << std::endl;

    return 0;
}
