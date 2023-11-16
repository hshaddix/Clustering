#include <iostream>
#include <vector>
#include <utility>
#include <cstdlib> // for strtol
#include <iomanip> // for std::hex and std::dec

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

int main(int argc, char *argv[]) {
    if (argc < 3 || argc % 2 == 0) {
        std::cerr << "Usage: " << argv[0] << " sum1 size1 sum2 size2 ..." << std::endl;
        return 1;
    }

    std::vector<std::pair<int, int>> clusters;

    for (int i = 1; i < argc; i += 2) {
        int sum = static_cast<int>(std::strtol(argv[i], nullptr, 16));
        int size = static_cast<int>(std::strtol(argv[i + 1], nullptr, 16));
        clusters.push_back(calculate_if_from_sum_size(sum, size));
    }

    std::vector<std::pair<int, int>> merged_clusters = merge_clusters(clusters);

    for (const auto& cluster : merged_clusters) {
        if (cluster.first != EMPTY_CLUSTER_SET_ERROR_CODE) {
            int sum = cluster.first + cluster.second;
            int size = cluster.second - cluster.first + 1;
            std::cout << "{" << std::hex << sum << "," << std::dec << size << "} ";
        }
    }
    std::cout << std::endl;

    return 0;
}
