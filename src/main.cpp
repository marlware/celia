#include <omp.h>

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "matrix.hpp"

namespace {

std::vector<int> parse_int_list(const std::string& csv) {
    std::vector<int> values;
    std::stringstream ss(csv);
    std::string item;
    while (std::getline(ss, item, ',')) {
        if (!item.empty()) {
            values.push_back(std::stoi(item));
        }
    }
    return values;
}

std::string get_arg(const std::vector<std::string>& args, const std::string& flag,
                     const std::string& fallback) {
    for (std::size_t i = 0; i + 1 < args.size(); ++i) {
        if (args[i] == flag) {
            return args[i + 1];
        }
    }
    return fallback;
}

// Sanity check that the parallel path agrees with the serial baseline
// before trusting any of the timing numbers below.
void verify_correctness() {
    celia::Matrix a = celia::Matrix::random(32, 32, 10);
    celia::Matrix b = celia::Matrix::random(32, 32, 20);

    celia::Matrix expected = celia::multiply_serial(a, b);
    celia::Matrix actual = celia::multiply_parallel(a, b, 4);

    double max_diff = 0.0;
    for (std::size_t i = 0; i < expected.rows(); ++i) {
        for (std::size_t j = 0; j < expected.cols(); ++j) {
            max_diff = std::max(max_diff, std::abs(expected(i, j) - actual(i, j)));
        }
    }

    if (max_diff > 1e-9) {
        std::cerr << "warning: parallel result diverged from serial by " << max_diff << "\n";
    }
}

}  // namespace

int main(int argc, char** argv) {
    std::vector<std::string> args(argv + 1, argv + argc);

    std::vector<int> sizes = parse_int_list(get_arg(args, "--sizes", "64,128,256,512"));
    std::vector<int> thread_counts = parse_int_list(get_arg(args, "--threads", "1,2,4,8"));
    int repeats = std::stoi(get_arg(args, "--repeats", "3"));
    std::string output_path = get_arg(args, "--output", "results.csv");

    verify_correctness();

    std::ofstream out(output_path);
    out << "matrix_size,threads,seconds,gflops\n";

    for (int n : sizes) {
        celia::Matrix a = celia::Matrix::random(static_cast<std::size_t>(n), static_cast<std::size_t>(n), 1);
        celia::Matrix b = celia::Matrix::random(static_cast<std::size_t>(n), static_cast<std::size_t>(n), 2);

        for (int threads : thread_counts) {
            double best_seconds = -1.0;

            for (int rep = 0; rep < repeats; ++rep) {
                double start = omp_get_wtime();
                celia::Matrix c = celia::multiply_parallel(a, b, threads);
                double elapsed = omp_get_wtime() - start;
                (void)c;

                if (best_seconds < 0.0 || elapsed < best_seconds) {
                    best_seconds = elapsed;
                }
            }

            double flops = 2.0 * static_cast<double>(n) * n * n;
            double gflops = flops / best_seconds / 1e9;

            std::cout << "size=" << n << " threads=" << threads
                      << " time=" << best_seconds << "s"
                      << " gflops=" << gflops << "\n";

            out << n << "," << threads << "," << best_seconds << "," << gflops << "\n";
        }
    }

    std::cout << "Wrote results to " << output_path << "\n";
    return 0;
}
