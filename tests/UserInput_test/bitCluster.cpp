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

    for (size_t i = 0; i < clusters.size(); ++i) {
        int current_i = clusters[i].first + offset * STRIP_SIZE;
        int current_f = clusters[i].second + offset * STRIP_SIZE;

        // Merge clusters if they are adjacent
        if (!merged.empty() && are_adjacent(merged.back(), {current_i, current_f})) {
            merged.back().second = current_f;
        } else {
            merged.push_back({current_i, current_f});
        }

        // If this is the last cluster in the strip, mark flag and increment offset
        if (last_in_strip) {
            offset++;
            last_in_strip = false;
        }

        // Check if this cluster is marked as the last in its strip
        if (i + 1 < clusters.size() && clusters[i + 1].first == 0) {
            last_in_strip = true;
        }
    }

    return merged;
}

int main() {
    std::vector<std::pair<int, int>> clusters;
    std::string binary_input;
    std::cin >> binary_input;

    for (size_t i = 0; i < binary_input.length(); i += 18) { // 16 bits per cluster + 2 spaces
        std::bitset<8> binary_sum(binary_input.substr(i, 8));
        std::bitset<8> binary_size(binary_input.substr(i + 9, 8)); // Skip the space
        bool is_last_in_strip = (i + 17 < binary_input.length()) && (binary_input[i + 17] == '1');

        clusters.push_back(calculate_if_from_sum_size(binary_sum.to_ulong(), binary_size.to_ulong()));

        // If this cluster is the last in its strip, process merging
        if (is_last_in_strip || i + 18 >= binary_input.length()) {
            clusters = merge_clusters(clusters);
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
