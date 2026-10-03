#pragma once

#include <cstddef>
#include <vector>

namespace celia {

// Simple row-major square-friendly dense matrix backed by a flat buffer.
class Matrix {
public:
    Matrix(std::size_t rows, std::size_t cols);

    static Matrix random(std::size_t rows, std::size_t cols, unsigned seed = 42);

    std::size_t rows() const { return rows_; }
    std::size_t cols() const { return cols_; }

    double& operator()(std::size_t r, std::size_t c);
    double operator()(std::size_t r, std::size_t c) const;

private:
    std::size_t rows_;
    std::size_t cols_;
    std::vector<double> data_;
};

// Baseline single-threaded multiply, used as the correctness/timing reference.
Matrix multiply_serial(const Matrix& a, const Matrix& b);

// OpenMP multiply that parallelizes over the output rows using num_threads threads.
Matrix multiply_parallel(const Matrix& a, const Matrix& b, int num_threads);

}  // namespace celia
