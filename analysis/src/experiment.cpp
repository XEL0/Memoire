#include <iostream>
#include <fstream>
#include <chrono>
#include <vector>
#include <memory>
#include <iomanip>
#include <cmath>
#include <map>
#include <random>

#include "ComparabilityBigraph.hpp"
#include "algorithms.hpp"

// AI generated

struct ExperimentResult {
    unsigned graph_size;
    unsigned p;
    unsigned q;
    unsigned dim;
    unsigned seed;
    double execution_time_ms;
    unsigned num_bicliques;
    unsigned total_decomposition_size;
    double avg_biclique_size;
    std::string algorithm_version;
};

struct Statistics {
    double mean;
    double std_dev;
    double min;
    double max;
};


Statistics computeStatistics(const std::vector<double>& times) {
    if (times.empty()) return {0, 0, 0, 0};

    double sum = 0.0;
    double min_val = times[0];
    double max_val = times[0];

    for (const auto& t : times) {
        sum += t;
        min_val = std::min(min_val, t);
        max_val = std::max(max_val, t);
    }

    const double mean = sum / times.size();

    double variance = 0.0;
    for (const auto& t : times) {
        variance += (t - mean) * (t - mean);
    }
    variance /= times.size();

    return {mean, std::sqrt(variance), min_val, max_val};
}


template<typename Func>
double measureExecutionTime(Func&& func) {
    const auto start = std::chrono::high_resolution_clock::now();
    func();
    const auto end = std::chrono::high_resolution_clock::now();

    const std::chrono::duration<double, std::milli> duration = end - start;
    return duration.count();
}

std::vector<ExperimentResult> runExperimentsWithSeeds(
    const std::vector<unsigned>& sizes,
    const std::vector<std::pair<double, double>>& partitions,
    const unsigned num_trials,
    const unsigned dim,
    const std::string& algorithm_version,
    const bool optimize_size,
    const std::vector<unsigned>& seeds
) {
    std::vector<ExperimentResult> results;
    GraphGenerator generator;

    unsigned seed_index = 0;

    for (const auto size : sizes) {
        for (const auto& [p_ratio, q_ratio] : partitions) {
            unsigned p = static_cast<unsigned>(size * p_ratio);
            unsigned q = static_cast<unsigned>(size * q_ratio);

            std::cout << "Testing " << algorithm_version << " - Size: " << size
                      << " (p=" << p << ", q=" << q << ")" << std::endl;

            for (unsigned trial = 0; trial < num_trials; ++trial) {
                unsigned seed = seeds[seed_index];
                seed_index++;

                const auto& vertices = generator.generate(p, q, dim, seed);
                auto graph = std::make_shared<ComparabilityBigraph>(
                    std::vector<Vertex*>(vertices.begin(), vertices.end()),
                    p, q, dim
                );

                std::vector<std::shared_ptr<ComparabilityBigraph>> bicliques;
                double exec_time = measureExecutionTime([&]() {
                    bicliques = Algorithms::partition(graph, optimize_size, 0);
                });

                unsigned total_size = 0;
                for (const auto& biclique_graph : bicliques) {
                    Biclique bc = biclique_graph->toBiclique();
                    total_size += bc.p() + bc.q();
                }

                double avg_size = bicliques.empty() ? 0.0 : static_cast<double>(total_size) / bicliques.size();

                ExperimentResult result;
                result.graph_size = size;
                result.p = p;
                result.q = q;
                result.dim = dim;
                result.seed = seed;
                result.execution_time_ms = exec_time;
                result.num_bicliques = bicliques.size();
                result.total_decomposition_size = total_size;
                result.avg_biclique_size = avg_size;
                result.algorithm_version = algorithm_version;

                results.push_back(result);

                std::cout << "  Trial " << (trial + 1) << "/" << num_trials
                          << " - Time: " << exec_time << " ms"
                          << " - Bicliques: " << bicliques.size()
                          << " - Total size: " << total_size
                          << " - Seed: " << seed << std::endl;
            }
        }
    }

    return results;
}

void saveRawResults(const std::vector<ExperimentResult>& results, const std::string& filename) {
    std::ofstream file(filename);

    if (!file.is_open()) {
        std::cerr << "ERROR: Could not open file " << filename << " for writing!" << std::endl;
        std::cerr << "Trying current directory instead..." << std::endl;
        std::string local_filename = "experiment_raw_results.csv";
        file.open(local_filename);

        if (!file.is_open()) {
            std::cerr << "ERROR: Could not create file in current directory either!" << std::endl;
            return;
        }
        std::cout << "Writing to current directory: " << local_filename << std::endl;
    }

    file << "algorithm_version,graph_size,p,q,dim,seed,execution_time_ms,num_bicliques,total_decomposition_size,avg_biclique_size\n";

    for (const auto& result : results) {
        file << result.algorithm_version << ","
             << result.graph_size << ","
             << result.p << ","
             << result.q << ","
             << result.dim << ","
             << result.seed << ","
             << std::fixed << std::setprecision(6) << result.execution_time_ms << ","
             << result.num_bicliques << ","
             << result.total_decomposition_size << ","
             << std::fixed << std::setprecision(2) << result.avg_biclique_size << "\n";
    }

    file.close();
    std::cout << "✓ Raw results saved to " << filename << std::endl;
}

void saveAggregatedResults(const std::vector<ExperimentResult>& results, const std::string& filename) {
    std::ofstream file(filename);

    if (!file.is_open()) {
        std::cerr << "ERROR: Could not open file " << filename << " for writing!" << std::endl;
        std::cerr << "Trying current directory instead..." << std::endl;

        std::string local_filename = "experiment_aggregated_results.csv";
        file.open(local_filename);

        if (!file.is_open()) {
            std::cerr << "ERROR: Could not create file in current directory either!" << std::endl;
            return;
        }
        std::cout << "Writing to current directory: " << local_filename << std::endl;
    }

    file << "algorithm_version,graph_size,p,q,mean_time_ms,std_dev_ms,min_time_ms,max_time_ms,mean_bicliques,std_dev_bicliques,mean_total_size,std_dev_total_size,mean_avg_biclique_size,std_dev_avg_biclique_size\n";

    // Group by (algorithm_version, graph_size, p, q)
    struct Key {
        std::string version;
        unsigned size;
        unsigned p;
        unsigned q;

        bool operator<(const Key& other) const {
            if (version != other.version) return version < other.version;
            if (size != other.size) return size < other.size;
            if (p != other.p) return p < other.p;
            return q < other.q;
        }
    };

    std::map<Key, std::vector<ExperimentResult>> grouped;
    for (const auto& result : results) {
        Key key{result.algorithm_version, result.graph_size, result.p, result.q};
        grouped[key].push_back(result);
    }

    for (const auto& [key, group] : grouped) {
        std::vector<double> times;
        std::vector<double> bicliques;
        std::vector<double> total_sizes;
        std::vector<double> avg_sizes;

        for (const auto& result : group) {
            times.push_back(result.execution_time_ms);
            bicliques.push_back(result.num_bicliques);
            total_sizes.push_back(result.total_decomposition_size);
            avg_sizes.push_back(result.avg_biclique_size);
        }

        auto time_stats = computeStatistics(times);
        auto biclique_stats = computeStatistics(bicliques);
        auto total_size_stats = computeStatistics(total_sizes);
        auto avg_size_stats = computeStatistics(avg_sizes);

        file << key.version << ","
             << key.size << ","
             << key.p << ","
             << key.q << ","
             << std::fixed << std::setprecision(6)
             << time_stats.mean << ","
             << time_stats.std_dev << ","
             << time_stats.min << ","
             << time_stats.max << ","
             << biclique_stats.mean << ","
             << biclique_stats.std_dev << ","
             << total_size_stats.mean << ","
             << total_size_stats.std_dev << ","
             << avg_size_stats.mean << ","
             << avg_size_stats.std_dev << "\n";
    }

    file.close();
    std::cout << "✓ Aggregated results saved to " << filename << std::endl;
}

int main() {
    const std::vector<unsigned> sizes = {5000, 10000, 20000, 40000, 80000, 160000};
    const std::vector<std::pair<double, double>> partitions = {
        {0.5, 0.5},   // Partitions égales
        {0.25, 0.75}, // p = 1/4, q = 3/4
        {0.75, 0.25}  // p = 3/4, q = 1/4
    };
    const unsigned num_trials = 10;
    const unsigned dim = 2; // Dimension des embeddings

    std::cout << "=================================================\n";
    std::cout << "Biclique Decomposition Performance Experiment\n";
    std::cout << "=================================================\n\n";
    std::cout << "Configuration:\n";
    std::cout << "  - Graph sizes: ";
    for (const auto s : sizes) std::cout << s << " ";
    std::cout << "\n  - Partitions: (0.5,0.5), (0.25,0.75), (0.75,0.25)\n";
    std::cout << "  - Trials per configuration: " << num_trials << "\n";
    std::cout << "  - Dimension: " << dim << "\n\n";

    std::cout << "Generating random seeds...\n";
    std::random_device rd;
    std::mt19937 seed_gen(rd());
    std::uniform_int_distribution<unsigned> seed_dist(0, 1000000);
    const unsigned total_configs = sizes.size() * partitions.size() * num_trials;
    std::vector<unsigned> seeds(total_configs);
    for (unsigned i = 0; i < total_configs; ++i) {
        seeds[i] = seed_dist(seed_gen);
    }

    std::cout << "Generated " << total_configs << " unique seeds\n\n";

    std::ofstream seed_file("/outputs/experiment_seeds.txt");
    if (!seed_file.is_open()) {
        std::cerr << "WARNING: Could not save seeds to /outputs/" << std::endl;
        seed_file.open("experiment_seeds.txt");
    }

    if (seed_file.is_open()) {
        seed_file << "# Seeds used for the experiment\n";
        seed_file << "# Format: index seed\n";
        for (unsigned i = 0; i < seeds.size(); ++i) {
            seed_file << i << " " << seeds[i] << "\n";
        }
        seed_file.close();
        std::cout << "Seeds saved\n\n";
    }

    // (optimize_size = false)
    std::cout << "\n### Running Version 1: Without size optimization ###\n\n";
    auto results_v1 = runExperimentsWithSeeds(sizes, partitions, num_trials, dim,
                                               "without_optimization", false, seeds);

    // (optimize_size = true)
    std::cout << "\n### Running Version 2: With size optimization ###\n\n";
    auto results_v2 = runExperimentsWithSeeds(sizes, partitions, num_trials, dim,
                                               "with_optimization", true, seeds);


    std::vector<ExperimentResult> all_results;
    all_results.insert(all_results.end(), results_v1.begin(), results_v1.end());
    all_results.insert(all_results.end(), results_v2.begin(), results_v2.end());

    std::string output_dir = "./";
    std::ofstream test_file("/outputs/.test");
    if (test_file.is_open()) {
        test_file.close();
        std::remove("/outputs/.test");
        output_dir = "/outputs/";
        std::cout << "\nUsing output directory: " << output_dir << std::endl;
    } else {
        std::cout << "\nCannot write to /outputs/, using current directory" << std::endl;
    }

    saveRawResults(all_results, output_dir + "experiment_raw_results.csv");
    saveAggregatedResults(all_results, output_dir + "experiment_aggregated_results.csv");
    std::cout << "\n=================================================\n";
    std::cout << "Experiment completed successfully!\n";
    std::cout << "=================================================\n";
    std::cout << "\nResults saved in: " << output_dir << std::endl;
    std::cout << "  - experiment_raw_results.csv\n";
    std::cout << "  - experiment_aggregated_results.csv\n";
    std::cout << "  - experiment_seeds.txt\n";

    return 0;
}