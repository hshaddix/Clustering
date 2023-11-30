#include <iostream>
#include <vector>
#include <bitset>
#include <string>

const int STRIP_SIZE = 126;
const int HIT_ADDRESS_BITS = 8;

std::pair<int, bool> parse_cluster(const std::string& binary) {
    std::bitset<HIT_ADDRESS_BITS> hit_address(std::string(binary.begin(), binary.end() - 1));
    bool is_last_cluster = binary.back() == '1';
    return {hit_address.to_ulong(), is_last_cluster};
}

std::pair<int, int> calculate_if_from_address(int address) {
    int size = 2; // Assuming size is always 2
    int f = (address + size - 1) / 2;
    int i = (address - size + 1) / 2;
    return {i, f};
}

bool are_adjacent(int f1, int i2) {
    return f1 == i2 - 1;
}

std::vector<std::pair<int, int>> merge_clusters(const std::vector<std::pair<int, bool>>& input_clusters) {
    std::vector<std::pair<int, int>> merged;
    int current_strip = 0;
    int start_i = -1;
    int end_f = -1;

    for (const auto& cluster : input_clusters) {
        int address = cluster.first;
        bool is_last_cluster = cluster.second;
        auto if_pair = calculate_if_from_address(address);
        int i = if_pair.first + current_strip * STRIP_SIZE;
        int f = if_pair.second + current_strip * STRIP_SIZE;

        if (start_i == -1) {
            start_i = i;
            end_f = f;
        } else if (are_adjacent(end_f, i)) {
            end_f = f;
        } else {
            merged.push_back({start_i, end_f});
            start_i = i;
            end_f = f;
        }

        if (is_last_cluster) {
            current_strip++;
        }
    }

    // Add the last cluster if not already added
    if (start_i != -1 && end_f != -1) {
        merged.push_back({start_i, end_f});
    }

    return merged;
}

int main() {
    std::vector<std::pair<int, bool>> clusters;
    std::string binary_input;

    // Read binary inputs (9-bit strings) from standard input
    while (std::cin >> binary_input) {
        clusters.push_back(parse_cluster(binary_input));
    }

    auto merged_clusters = merge_clusters(clusters);

    for (const auto& cluster : merged_clusters) {
        std::cout << "{" << std::hex << cluster.first << ", " << std::dec << cluster.second - cluster.first + 1 << "} ";
    }
    std::cout << std::endl;

    return 0;
}
