#include <iostream>
#include <vector>
#include <bitset>
#include <string>

const int STRIP_SIZE = 126;
const int HIT_ADDRESS_BITS = 8;

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
    int previous_f = -1;

    for (size_t i = 0; i < clusters.size(); ++i) {
        int current_i = clusters[i].first + offset * STRIP_SIZE;
        int current_f = clusters[i].second + offset * STRIP_SIZE;

        if (!merged.empty() && are_adjacent(merged.back(), {current_i, current_f})) {
            merged.back().second = current_f;
        } else {
            merged.push_back({current_i, current_f});
        }

        if (i + 1 < clusters.size() && clusters[i + 1].first == 0) {
            offset++;
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
        auto cluster = calculate_if_from_sum_size(sum, size);
        clusters.push_back(cluster);

        if (binary_input[8] == '1') {  // Last cluster in the strip
            auto merged_clusters = merge_clusters(clusters);
            clusters.clear();

            for (const auto& cluster : merged_clusters) {
                int sum = cluster.first + cluster.second;
                int size = cluster.second - cluster.first + 1;
                std::bitset<8> binary_sum_output(sum);
                std::bitset<8> binary_size_output(size);
                std::cout << "{" << binary_sum_output.to_string() << "," << binary_size_output.to_string() << "} ";
            }
            std::cout << std::endl;
        }
    }

    return 0;
}
