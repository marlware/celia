#include "matrix.hpp"

#include <omp.h>

#include <random>
#include <stdexcept>

namespace celia {

Matrix::Matrix(std::size_t rows, std::size_t cols)
    : rows_(rows), cols_(cols), data_(rows * cols, 0.0) {}

Matrix Matrix::random(std::size_t rows, std::size_t cols, unsigned seed) {
    Matrix m(rows, cols);
    std::mt19937 gen(seed);
    std::uniform_real_distribution<double> dist(-1.0, 1.0);
    for (std::size_t i = 0; i < rows * cols; ++i) {
        m.data_[i] = dist(gen);
    }
    return m;
}

double& Matrix::operator()(std::size_t r, std::size_t c) {
    return data_[r * cols_ + c];
}

double Matrix::operator()(std::size_t r, std::size_t c) const {
    return data_[r * cols_ + c];
}

Matrix multiply_serial(const Matrix& a, const Matrix& b) {
    if (a.cols() != b.rows()) {
        throw std::invalid_argument("multiply_serial: dimension mismatch");
    }

    Matrix result(a.rows(), b.cols());
    for (std::size_t i = 0; i < a.rows(); ++i) {
        for (std::size_t k = 0; k < a.cols(); ++k) {
            double a_ik = a(i, k);
            for (std::size_t j = 0; j < b.cols(); ++j) {
                result(i, j) += a_ik * b(k, j);
            }
        }
    }
    return result;
}

Matrix multiply_parallel(const Matrix& a, const Matrix& b, int num_threads) {
    if (a.cols() != b.rows()) {
        throw std::invalid_argument("multiply_parallel: dimension mismatch");
    }

    Matrix result(a.rows(), b.cols());
    const long rows = static_cast<long>(a.rows());
    const std::size_t inner = a.cols();
    const std::size_t cols = b.cols();

#pragma omp parallel for num_threads(num_threads) schedule(static)
    for (long i = 0; i < rows; ++i) {
        for (std::size_t k = 0; k < inner; ++k) {
            double a_ik = a(static_cast<std::size_t>(i), k);
            for (std::size_t j = 0; j < cols; ++j) {
                result(static_cast<std::size_t>(i), j) += a_ik * b(k, j);
            }
        }
    }
    return result;
}

}  // namespace celia
