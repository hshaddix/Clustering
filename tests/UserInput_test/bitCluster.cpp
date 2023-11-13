#include <iostream>
#include <vector>
#include <utility>
#include <cstdlib> // for strtol
#include <iomanip> // for std::hex and std::dec

const int EMPTY_CLUSTER_SET = 0; // Indicator of an empty cluster set
const int EMPTY_CLUSTER_SET_ERROR_CODE = 0xFE; // Error code for an empty cluster set
const int STRIP_SIZE = 126; // The size of your strips

std::pair<int, int> calculate_if_from_sum_size(int sum, int size) {
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
    int offset = 0; // Initialize offset for the first strip

    for (size_t i = 0; i < clusters.size(); ++i) {
        int current_i = clusters[i].first;
        int current_f = clusters[i].second;

        // Handle empty cluster set and increment offset
        if (current_i == EMPTY_CLUSTER_SET && current_f == EMPTY_CLUSTER_SET) {
            offset++;
            continue; // Skip further processing for this empty strip
        }

        current_i += offset * STRIP_SIZE;
        current_f += offset * STRIP_SIZE;

        while (i + 1 < clusters.size() && are_adjacent(clusters[i], clusters[i + 1], offset)) {
            i++; // Skip the next cluster as it has been merged
            int next_cluster_f = clusters[i + 1].second + offset * STRIP_SIZE;
            current_f = next_cluster_f;
            if (clusters[i].second == STRIP_SIZE - 1) { // Increment offset for the last cluster in the strip
                offset++;
            }
        }

        merged.push_back({(current_f - offset * STRIP_SIZE - current_i + 1) / 2 + current_i, current_f - offset * STRIP_SIZE - current_i + 1});
    }

    return merged;
}

int main(int argc, char *argv[]) {
    if (argc < 3 || argc % 2 == 0) {
        std::cerr << "Usage: " << argv[0] << " sum1 size1 sum2 size2 ..." << std::endl;
        return 1;
    }

    std::vector<std::pair<int, int>> clusters;

    // Read clusters from command line arguments as hexadecimal values
    for (int i = 1; i < argc; i += 2) {
        int sum = static_cast<int>(std::strtol(argv[i], nullptr, 16));
        int size = static_cast<int>(std::strtol(argv[i + 1], nullptr, 16));
        clusters.push_back(calculate_if_from_sum_size(sum, size));
    }

    std::vector<std::pair<int, int>> merged_clusters = merge_clusters(clusters);

    // Output the merged clusters and handle empty strips
    for (const auto& cluster : merged_clusters) {
        if (cluster.first == EMPTY_CLUSTER_SET && cluster.second == EMPTY_CLUSTER_SET) {
            std::cout << "$" << std::hex << EMPTY_CLUSTER_SET_ERROR_CODE << " ";
        } else {
            int sum = (cluster.second - cluster.first + 1) / 2 + cluster.first;
            int size = cluster.second - cluster.first + 1;
            std::cout << "{" << std::hex << sum << "," << std::dec << size << "} ";
        }
    }
    std::cout << std::endl;

    return 0;
}
