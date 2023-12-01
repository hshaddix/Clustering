#include <iostream>
#include <vector>
#include <bitset>
#include <string>

const int STRIP_SIZE = 126;
const int EMPTY_CLUSTER_SET = 0xFE; // Hexadecimal FE to represent an empty cluster

std::pair<int, int> calculate_if_from_sum_size(int sum, int size) {
    if (sum == 0 && size == 1) {
        return {EMPTY_CLUSTER_SET, EMPTY_CLUSTER_SET}; // Mark as an empty cluster
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

    for (size_t i = 0; i < clusters.size(); ++i) {
        if (clusters[i].first == EMPTY_CLUSTER_SET) {
            // Handle empty strip
            offset++; // Increment offset for next strip
            continue;
        }

        int current_i = clusters[i].first + offset * STRIP_SIZE;
        int current_f = clusters[i].second + offset * STRIP_SIZE;

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

        if (cluster.first == EMPTY_CLUSTER_SET) {
            std::cout << "FE ";
            continue; // Skip merging for empty strip
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
