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

bool are_adjacent(int f1, int i2) {
    return f1 + 1 == i2;
}

std::vector<std::pair<int, int>> merge_clusters(const std::vector<std::pair<int, int>>& clusters) {
    std::vector<std::pair<int, int>> merged;
    int offset = 0;

    for (size_t i = 0; i < clusters.size(); ++i) {
        int current_i = clusters[i].first + offset * STRIP_SIZE;
        int current_f = clusters[i].second + offset * STRIP_SIZE;

        if (!merged.empty() && are_adjacent(merged.back().second, current_i)) {
            merged.back().second = current_f;
        } else {
            merged.push_back({current_i, current_f});
        }

        if (clusters[i].second == -1 || (i < clusters.size() - 1 && clusters[i + 1].first == -1)) {
            offset++;
        }
    }

    return merged;
}

std::string format_binary_output(int num) {
    std::bitset<8> binary(num);
    return binary.to_string();
}

int main() {
    std::vector<std::pair<int, int>> clusters;
    std::string binary_input;

    while (std::cin >> binary_input) {
        std::bitset<8> binary_sum(binary_input.substr(0, 8));
        std::bitset<8> binary_size(binary_input.substr(8, 8));
        bool is_last_in_strip = binary_input[16] == '1';

        if (binary_sum.to_ulong() == 0 && binary_size.to_ulong() == 0) {
            clusters.push_back({-1, -1}); // Empty strip indicator
            continue;
        }

        clusters.push_back(calculate_if_from_sum_size(binary_sum.to_ulong(), binary_size.to_ulong()));

        if (is_last_in_strip) {
            clusters.push_back({-1, -1}); // Mark end of strip
        }
    }

    auto merged_clusters = merge_clusters(clusters);

    for (const auto& cluster : merged_clusters) {
        if (cluster.first != -1) {
            int sum = cluster.first + cluster.second;
            int size = cluster.second - cluster.first + 1;
            std::cout << "{" << format_binary_output(sum) << "," << format_binary_output(size) << "} ";
        } else {
            std::cout << "FE ";
        }
    }
    std::cout << std::endl;

    return 0;
}
