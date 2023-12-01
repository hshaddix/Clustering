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

std::vector<std::pair<int, int>> merge_clusters(const std::vector<std::pair<int, int>>& clusters) {
    std::vector<std::pair<int, int>> merged;
    int offset = 0;

    for (size_t i = 0; i < clusters.size(); ++i) {
        if (clusters[i].first == -1 && clusters[i].second == -1) {
            // Handle empty strip
            if (merged.empty() || merged.back().first != -1) {
                merged.push_back({-1, -1});
            }
            offset++;
            continue;
        }

        int current_i = clusters[i].first + offset * STRIP_SIZE;
        int current_f = clusters[i].second + offset * STRIP_SIZE;

        if (!merged.empty() && merged.back().second == current_i - 1) {
            merged.back().second = current_f; // Merge with previous cluster
        } else {
            merged.push_back({current_i, current_f});
        }
    }

    return merged;
}

int main() {
    std::vector<std::pair<int, int>> clusters;
    std::string binary_input;

    while (std::cin >> binary_input) {
        if (binary_input == "00000000000000001") {
            // Handle empty strip
            clusters.push_back({-1, -1});
            continue;
        }

        std::bitset<8> binary_sum(binary_input.substr(0, 8));
        std::bitset<8> binary_size(binary_input.substr(8, 8));
        auto cluster = calculate_if_from_sum_size(binary_sum.to_ulong(), binary_size.to_ulong());
        clusters.push_back(cluster);

        if (binary_input[16] == '1') {
            clusters.push_back({-1, -1}); // Indicate end of strip
        }
    }

    auto merged_clusters = merge_clusters(clusters);

    for (const auto& cluster : merged_clusters) {
        if (cluster.first == -1 && cluster.second == -1) {
            std::cout << "FE ";
            continue;
        }

        int sum = cluster.first + cluster.second;
        int size = cluster.second - cluster.first + 1;
        std::bitset<8> binary_sum(sum);
        std::bitset<8> binary_size(size);
        std::cout << "{" << binary_sum.to_string() << "," << binary_size.to_string() << "} ";
    }
    std::cout << std::endl;

    return 0;
}
