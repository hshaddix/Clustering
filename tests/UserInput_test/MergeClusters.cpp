#include <iostream>
#include <vector>
#include <utility>
#include <cstdlib> // for atoi

std::pair<int, int> calculate_if_from_sum_size(int sum, int size) {
    int f = (sum + size - 1) / 2;
    int i = (sum - size + 1) / 2;
    return {i, f};
}

bool are_adjacent(const std::pair<int, int>& cluster1, const std::pair<int, int>& cluster2) {
    return cluster1.second + 1 == cluster2.first;
}

std::vector<std::pair<int, int>> merge_clusters(const std::vector<std::pair<int, int>>& clusters) {
    std::vector<std::pair<int, int>> merged;
    for (size_t i = 0; i < clusters.size(); ++i) {
        int current_i = clusters[i].first;
        int current_f = clusters[i].second;
        
        while (i + 1 < clusters.size() && are_adjacent(clusters[i], clusters[i + 1])) {
            current_f = clusters[i + 1].second;
            ++i;
        }
        
        merged.push_back({current_i, current_f});
    }
    return merged;
}

int main(int argc, char *argv[]) {
    if (argc < 3 || argc % 2 == 0) {
        std::cerr << "Usage: " << argv[0] << " sum1 size1 sum2 size2 ..." << std::endl;
        return 1;
    }

    std::vector<std::pair<int, int>> clusters_if;

    // Read clusters from command line arguments
    for (int i = 1; i < argc; i += 2) {
        int sum = std::atoi(argv[i]);
        int size = std::atoi(argv[i + 1]);
        clusters_if.push_back(calculate_if_from_sum_size(sum, size));
    }

    // Sort clusters by initial value (i)
    sort(clusters_if.begin(), clusters_if.end());

    // Merge adjacent clusters
    auto merged_clusters = merge_clusters(clusters_if);

    // Convert the merged clusters back to {sum, size} format
    for (const auto& cluster : merged_clusters) {
        int sum = cluster.first + cluster.second;
        int size = cluster.second - cluster.first + 1;
        std::cout << "{" << sum << "," << size << "} ";
    }
    std::cout << std::endl;

    return 0;
}
