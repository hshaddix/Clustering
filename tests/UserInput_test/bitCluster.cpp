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

std::vector<std::pair<int, int>> merge_clusters(const std::vector<std::pair<int, int>>& clusters, bool is_last_in_strip) {
    std::vector<std::pair<int, int>> merged;
    int offset = 0;

    for (size_t i = 0; i < clusters.size(); ++i) {
        int current_i = clusters[i].first;
        int current_f = clusters[i].second;

        if (i > 0 && is_last_in_strip) {
            current_i += STRIP_SIZE;
            current_f += STRIP_SIZE;
        }

        if (!merged.empty() && are_adjacent(merged.back(), {current_i, current_f})) {
            merged.back().second = current_f;
        } else {
            merged.push_back({current_i, current_f});
        }

        if (is_last_in_strip) {
            offset++;
            is_last_in_strip = false;  // Reset for next cluster
        }
    }

    return merged;
}

int main() {
    std::vector<std::pair<int, int>> clusters;
    std::string binary_input;
    std::cin >> binary_input;

    bool is_last_in_strip = false;
    for (size_t i = 0; i < binary_input.length(); i += 18) {  // Each cluster is 17 bits + 1 space
        std::bitset<8> binary_sum(binary_input.substr(i, 8));
        std::bitset<8> binary_size(binary_input.substr(i + 9, 8)); // Skip the space
        is_last_in_strip = (i + 17 < binary_input.length() && binary_input[i + 17] == '1'); // Check if it's the last cluster in a strip

        clusters.push_back(calculate_if_from_sum_size(binary_sum.to_ulong(), binary_size.to_ulong()));

        // Process merging at the end of each strip or at the end of input
        if (is_last_in_strip || i + 18 >= binary_input.length()) {
            auto merged_clusters = merge_clusters(clusters, is_last_in_strip);
            clusters = merged_clusters;
        }
    }

    for (const auto& cluster : clusters) {
        int sum = cluster.first + cluster.second;
        int size = cluster.second - cluster.first + 1;
        std::bitset<8> binary_sum_output(sum);
        std::bitset<8> binary_size_output(size);
        std::cout << "{" << binary_sum_output.to_string() << "," << binary_size_output.to_string() << "} ";
    }
    std::cout << std::endl;

    return 0;
}
