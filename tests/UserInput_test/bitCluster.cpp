#include <iostream>
#include <vector>
#include <bitset>
#include <string>

const int STRIP_SIZE = 126;

std::pair<int, int> calculate_if_from_sum_size(int sum, int size) {
    if (sum == 0 && size == 0) {
        return {-1, -1}; // Mark as an empty cluster
    }
    int f = (sum + size - 1) / 2;
    int i = (sum - size + 1) / 2;
    return {i, f};
}

std::vector<std::pair<int, int>> merge_clusters(const std::vector<std::pair<int, int>>& clusters) {
    std::vector<std::pair<int, int>> merged;
    int offset = 0;

    for (const auto& cluster : clusters) {
        if (cluster.first == -1 && cluster.second == -1) {
            std::cout << "FE "; // Output FE for empty strip
            offset++; // Increment offset for the next strip
            continue;
        }

        int adjusted_i = cluster.first + offset * STRIP_SIZE;
        int adjusted_f = cluster.second + offset * STRIP_SIZE;

        // Merge if adjacent
        if (!merged.empty() && merged.back().second == adjusted_i - 1) {
            merged.back().second = adjusted_f;
        } else {
            merged.push_back({adjusted_i, adjusted_f});
        }
    }

    return merged;
}

int main() {
    std::vector<std::pair<int, int>> clusters;
    std::string binary_input;

    while (std::cin >> binary_input) {
        std::bitset<8> binary_sum(binary_input.substr(0, 8));
        std::bitset<8> binary_size(binary_input.substr(8, 8));
        auto cluster = calculate_if_from_sum_size(binary_sum.to_ulong(), binary_size.to_ulong());
        
        if (cluster.first == -1 && cluster.second == -1) {
            continue; // Skip processing for empty strip
        }

        clusters.push_back(cluster);
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
