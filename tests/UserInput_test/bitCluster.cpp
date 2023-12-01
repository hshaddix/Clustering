#include <iostream>
#include <vector>
#include <bitset>
#include <string>

const int STRIP_SIZE = 126;

std::pair<int, int> calculate_if_from_sum_size(int sum, int size) {
    // Special case for an empty strip
    if (sum == 0 && size == 1) {
        return {0, -1};
    }
    int f = (sum + size - 1) / 2;
    int i = (sum - size + 1) / 2;
    return {i, f};
}

bool are_adjacent(const std::pair<int, int>& cluster1, const std::pair<int, int>& cluster2) {
    return cluster1.second + 1 == cluster2.first;
}

std::vector<std::pair<int, int>> merge_clusters(const std::vector<std::pair<int, int>>& clusters) {
    std::vector<std::pair<int, int>> merged;
    int offset = 0;

    for (const auto& cluster : clusters) {
        if (cluster.second == -1) {  // Handle empty strip
            offset++; // Increment offset for the next strip
            continue;
        }

        int current_i = cluster.first + offset * STRIP_SIZE;
        int current_f = cluster.second + offset * STRIP_SIZE;

        if (!merged.empty() && are_adjacent(merged.back(), {current_i, current_f})) {
            merged.back().second = current_f;
        } else {
            merged.push_back({current_i, current_f});
        }
    }

    return merged;
}

int main() {
    std::vector<std::pair<int, int>> clusters;
    std::string binary_input;
    std::cin >> binary_input;

    for (size_t i = 0; i < binary_input.length(); i += 16) { // 16 bits per cluster
        std::bitset<8> binary_sum(binary_input.substr(i, 8));
        std::bitset<8> binary_size(binary_input.substr(i + 8, 8));
        auto cluster = calculate_if_from_sum_size(binary_sum.to_ulong(), binary_size.to_ulong());
        
        if (cluster.second == -1) {
            std::cout << "FE "; // Output FE for empty strip
            continue;
        }

        clusters.push_back(cluster);
    }

    auto merged_clusters = merge_clusters(clusters);

    for (const auto& cluster : merged_clusters) {
        if (cluster.first == 0 && cluster.second == -1) {
            continue; // Skip output for empty strip
        }

        int sum = cluster.first + cluster.second;
        int size = cluster.second - cluster.first + 1;
        std::bitset<8> binary_sum_output(sum);
        std::bitset<8> binary_size_output(size);
        std::cout << "{" << binary_sum_output.to_string() << "," << binary_size_output.to_string() << "} ";
    }
    std::cout << std::endl;

    return 0;
}
