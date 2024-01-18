#include <iostream>
#include <vector>
#include <bitset>
#include <string>

const int STRIP_SIZE = 126;

std::pair<int, int> calculate_if_from_initial_size(int initial, int size) {
    int f = initial + size - 1;
    return {initial, f};
}

std::vector<std::pair<int, int>> merge_clusters(const std::vector<std::pair<int, int>>& clusters) {
    std::vector<std::pair<int, int>> merged;

    for (size_t i = 0; i < clusters.size(); ++i) {
        int current_i = clusters[i].first;
        int current_f = clusters[i].second;

        if (!merged.empty() && merged.back().second + 1 == current_i) {
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
        if (binary_input == "0000000000000000") {
            // Handle empty strip
            continue;
        }

        int strip_number = std::stoi(binary_input.substr(1, 4), nullptr, 2);
        int initial_hit = std::stoi(binary_input.substr(5, 8), nullptr, 2);
        int size = std::stoi(binary_input.substr(13, 3), nullptr, 2);

        auto cluster = calculate_if_from_initial_size(initial_hit + strip_number * STRIP_SIZE, size);
        clusters.push_back(cluster);
    }

    auto merged_clusters = merge_clusters(clusters);

    for (const auto& cluster : merged_clusters) {
        int initial = cluster.first;
        int final = cluster.second;
        int size = final - initial + 1;
        std::cout << "{" << initial << "," << size << "} ";
    }
    std::cout << std::endl;

    return 0;
}
