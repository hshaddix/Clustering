#include <iostream>
#include <vector>
#include <utility>
#include <cstdlib> // for strtol
#include <iomanip> // for std::hex and std::dec

const int EMPTY_CLUSTER_SET = 0; // Indicator of an empty cluster set
const int EMPTY_CLUSTER_SET_ERROR_CODE = 0xFE; // Error code for an empty cluster set
const int STRIP_SIZE = 126; // The size of your strips

// Function to calculate the initial (i) and final (f) address of a cluster
std::pair<int, int> calculate_if_from_sum_size(int sum, int size_without_flag) {
    int f = (sum + size_without_flag - 1) / 2;
    int i = (sum - size_without_flag + 1) / 2;
    return {i, f};
}

// Function to check if two clusters are adjacent considering the strip offset
bool are_adjacent(const std::pair<int, int>& cluster1, const std::pair<int, int>& cluster2, int offset) {
    return cluster1.second + 1 == cluster2.first + offset * STRIP_SIZE;
}

// Function to merge clusters taking into account the strip offset
std::vector<std::pair<int, int>> merge_clusters(const std::vector<std::pair<int, int>>& clusters) {
    std::vector<std::pair<int, int>> merged;
    int offset = 0; // Initialize offset for the first strip

    for (size_t i = 0; i < clusters.size(); ++i) {
        int current_start = clusters[i].first;
        int current_end = clusters[i].first + (clusters[i].second & ~1) - 1;

        // Increment offset if it's the last cluster in the strip
        bool is_last_in_strip = clusters[i].second & 1;
        if (is_last_in_strip) {
            offset++;
        }

        // Check if adjacent and merge
        while (i + 1 < clusters.size() && are_adjacent({current_start, current_end}, clusters[i + 1], offset)) {
            current_end = clusters[i + 1].first + (clusters[i + 1].second & ~1) - 1 + offset * STRIP_SIZE;
            i++; // Skip the next cluster as it has been merged
            if (clusters[i].second & 1) { // Check if this is the last cluster in its strip
                offset++;
            }
        }

        merged.push_back({current_start, current_end});
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
        clusters.push_back({sum, size});
    }

    // Merge adjacent clusters considering offsets implied by the LSB flag
    std::vector<std::pair<int, int>> merged_clusters = merge_clusters(clusters);

    // Convert the merged clusters back to {sum, size} format and output as hex
    for (const auto& cluster : merged_clusters) {
        if (cluster.first != EMPTY_CLUSTER_SET || cluster.second != EMPTY_CLUSTER_SET) {
            int sum = cluster.first + cluster.second;
            int size = cluster.second - cluster.first + 1;
            std::cout << "{" << std::hex << sum << "," << std::dec << size << "} ";
        }
    }
    std::cout << std::endl;

    return 0;
}
