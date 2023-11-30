#include <iostream>
#include <vector>
#include <bitset>
#include <string>

const int EMPTY_CLUSTER_SET = 0;
const int EMPTY_CLUSTER_SET_ERROR_CODE = 0xFE;
const int STRIP_SIZE = 126;

std::pair<int, int> calculate_if_from_sum_size(int sum, int size) {
    if (sum == 0 && size == 0) {
        return {EMPTY_CLUSTER_SET_ERROR_CODE, EMPTY_CLUSTER_SET_ERROR_CODE};
    }
    int f = (sum + size - 1) / 2;
    int i = (sum - size + 1) / 2;
    return {i, f};
}

bool are_adjacent(const std::pair<int, int>& cluster1, const std::pair<int, int>& cluster2, int offset) {
    int cluster2_start_with_offset = cluster2.first + offset * STRIP_SIZE;
    return cluster1.second == cluster2_start_with_offset - 1;
}

std::vector<std::pair<int, int>> merge_clusters(const std::vector<std::pair<int, int>>& clusters) {
    std::vector<std::pair<int, int>> merged;
    int offset = 0;
    int previous_f = -1;

    for (size_t i = 0; i < clusters.size(); ++i) {
        if (clusters[i].first == EMPTY_CLUSTER_SET_ERROR_CODE) {
            std::cout << "FE "; // Output FE for empty strip
            offset++;
            continue;
        }

        int current_i = clusters[i].first + offset * STRIP_SIZE;
        int current_f = clusters[i].second + offset * STRIP_SIZE;

        if (previous_f > current_i) {
            offset++;
            current_i += STRIP_SIZE;
            current_f += STRIP_SIZE;
        }
        previous_f = current_f;

        if (!merged.empty() && are_adjacent(merged.back(), {current_i, current_f}, 0)) {
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

    // Read binary inputs (9-bit strings) from standard input
    while (std::cin >> binary_input) {
        std::bitset<8> binary_sum(binary_input.substr(0, 8));
        int sum = static_cast<int>(binary_sum.to_ulong());
        int size = 2;  // Assuming size is always 2
        bool is_last_cluster = binary_input[8] == '1';
        
        auto cluster = calculate_if_from_sum_size(sum, size);
        if (cluster.first != EMPTY_CLUSTER_SET_ERROR_CODE) {
            clusters.push_back(cluster);
        }

        // Handle last cluster in the strip
        if (is_last_cluster) {
            auto merged_clusters = merge_clusters(clusters);
            clusters.clear();

            for (const auto& cluster : merged_clusters) {
                std::bitset<8> binary_sum(cluster.first + cluster.second);
                int size = cluster.second - cluster.first + 1;
                std::cout << "{" << binary_sum.to_string() << "," << std::bitset<8>(size).to_string() << "} ";
            }
            std::cout << std::endl;
        }
    }

    return 0;
}
