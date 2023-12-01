#include <iostream>
#include <vector>
#include <bitset>
#include <string>

const int STRIP_SIZE = 126;

std::pair<int, int> calculate_if_from_sum_size(int sum, int size) {
    if (sum == 0 && size == 1) {
        return {0, -1};  // Special indicator for an empty strip
    }
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
        if (clusters[i].second == -1) { // Empty strip
            offset++;
            continue;
        }

        int current_i = clusters[i].first + offset * STRIP_SIZE;
        int current_f = clusters[i].second + offset * STRIP_SIZE;

        if (!merged.empty() && are_adjacent(merged.back().second, current_i)) {
            merged.back().second = current_f;
        } else {
            merged.push_back({current_i, current_f});
        }
    }

    return merged;
}

std::string format_output(int sum, int size) {
    std::bitset<8> binary_sum(sum);
    std::bitset<8> binary_size(size);
    return "{" + binary_sum.to_string() + "," + binary_size.to_string() + "} ";
}

int main() {
    std::vector<std::pair<int, int>> clusters;
    std::string binary_input;

    while (std::cin >> binary_input) {
        std::bitset<8> binary_sum(binary_input.substr(0, 8));
        std::bitset<8> binary_size(binary_input.substr(8, 8));
        auto cluster = calculate_if_from_sum_size(binary_sum.to_ulong(), binary_size.to_ulong());

        if (cluster.second == -1) {
            std::cout << "FE ";
            continue;
        }

        clusters.push_back(cluster);
    }

    auto merged_clusters = merge_clusters(clusters);

    for (const auto& cluster : merged_clusters) {
        if (cluster.first != 0 || cluster.second != -1) {
            int sum = cluster.first + cluster.second;
            int size = cluster.second - cluster.first + 1;
            std::cout << format_output(sum, size);
        }
    }
    std::cout << std::endl;

    return 0;
}
