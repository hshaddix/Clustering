#include <iostream>
#include <vector>
#include <bitset>
#include <string>

const int STRIP_SIZE = 126;

std::pair<int, int> calculate_global_if_from_local(int strip, int initial, int size) {
    int global_initial = initial + strip * STRIP_SIZE;
    int global_final = global_initial + size - 1;
    return {global_initial, global_final};
}

std::pair<int, int, int> calculate_local_if_from_global(int global_initial, int global_final) {
    int strip = global_initial / STRIP_SIZE;
    int local_initial = global_initial % STRIP_SIZE;
    int size = global_final - global_initial + 1;
    return {strip, local_initial, size};
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

std::string format_output(int strip, int initial, int size) {
    std::bitset<4> strip_bits(strip);
    std::bitset<8> initial_bits(initial);
    std::bitset<3> size_bits(size - 1); // size - 1, because size range is 1-8
    return "0" + strip_bits.to_string() + initial_bits.to_string() + size_bits.to_string();
}

int main() {
    std::vector<std::pair<int, int>> clusters;
    std::string binary_input;

    while (std::cin >> binary_input) {
        if (binary_input == "0000000000000000") {
            // Handle empty strip
            std::cout << "0000000000000000 ";
            continue;
        }

        int strip_number = std::stoi(binary_input.substr(1, 4), nullptr, 2);
        int initial_hit = std::stoi(binary_input.substr(5, 8), nullptr, 2);
        int size = std::stoi(binary_input.substr(13, 3), nullptr, 2);

        auto cluster = calculate_global_if_from_local(strip_number, initial_hit, size);
        clusters.push_back(cluster);
    }

    auto merged_clusters = merge_clusters(clusters);

    for (const auto& cluster : merged_clusters) {
        auto [strip, local_initial, size] = calculate_local_if_from_global(cluster.first, cluster.second);
        std::cout << format_output(strip, local_initial, size) << " ";
    }
    std::cout << std::endl;

    return 0;
}
