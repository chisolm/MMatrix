// Copyright [2018] <Chris Chisolm>
#ifndef _MMATRIX_HPP
#define _MMATRIX_HPP

#include <vector>
#include <string>
#include <algorithm>

template <typename T> class MMatrix {
    unsigned rows_;
    unsigned cols_;
    std::vector<T> vec;

public:
    MMatrix() : rows_(0), cols_(0), vec(0) {}
    MMatrix(unsigned r, unsigned c, const T& t = 0.0) :
            rows_(r),
            cols_(c),
            vec(r * c, t) {}

    // Access func for size
    unsigned rows() const { return rows_; }
    unsigned cols() const { return cols_; }

    // Convience initializer, example:
    //     MMatrix mm32(2, 2);
    //     mm32 = {7, 10, 9, 12};
    MMatrix& operator=(std::initializer_list<T> a) {
        std::copy_n(begin(a), std::min(a.size(), vec.size()), vec.begin());
        return (*this);
    }

    // Element access functions, non-const and const
    //     std::cout << mmc(0,0) << std::endl;
    inline T& operator() (unsigned row, unsigned col) {
        // if (row >= rows_ || col >= cols_)
        //     throw std::out_of_range("MMatrix subscript out of bounds");
        return vec[cols_ * row + col];
    }
    inline const T& operator() (unsigned row, unsigned col) const {
        // if (row >= rows_ || col >= cols_)
        //     throw std::out_of_range("MMatrix subscript out of bounds");
        return vec[cols_ * row + col];
    }


    // Mulitplication and cummulative multiplication
    friend MMatrix<T> operator*(const MMatrix<T>& lhs, const MMatrix<T>& rhs) {
        if (lhs.cols_ != rhs.rows_) {
            throw std::out_of_range(
                std::string("Matrix dimensions incorrect for multiplication lhs: ") +
                lhs.dim_string() + std::string("rhs dim: ") + rhs.dim_string());
        }


        MMatrix<T> result = MMatrix(lhs.rows_, rhs.cols_, 0);

        // Comments on optimization
        //   1. Re-ordering the loops for the naive 'i, j, k' to 'i, k, j' has
        //   an enourmous benefit in performance.  10x 1kx1k test case 90s->12sec
        //   2. Precomputing the row offsets has no benefit, likely the compiler
        //   is doing this for us.
        //   3. Using a second set of 3 loops to support 'block' locality of access
        //   had a negative affect.  12sec->13sec @32 block size was the least bad.
        //   'i, k, j' loop order on both loops was required.
        //   5. Using a '#pragma unroll 4' for inner loop unrolling had no benefit at 4,
        //   and was worse at 8 and 16.
        //   4. Transpose of the rhs prior to multiplication was no additional help
        //   over the 'i, k, j' loop order.
        for (unsigned i = 0; i < lhs.rows_; i++) {   // rows_ of output matrix
            // No actual benefit to precomputing, likely compiler does this by default.
            unsigned ir = i * lhs.rows_;
            unsigned ic = i * lhs.cols_;
            // huge benefit to moving k loop outside j - 94secs -> 12
            for (unsigned k = 0; k < lhs.cols_; k++) {
                unsigned kc = k * rhs.cols_;
                for (unsigned j = 0; j < rhs.cols_; j++) {
                    result.vec[ir + j] += lhs.vec[ic + k] * rhs.vec[kc + j];
                }
            }
        }
        return result;
    }
    MMatrix<T>& operator*=(const MMatrix<T>& rhs) {
        MMatrix<T> result = (*this) * rhs;
        (*this) = result;
        return *this;
    }

    // Addition operator added for test case use
    MMatrix<T> operator+(const MMatrix<T>& rhs) {
        MMatrix<T> result(rows_, cols_, 0.0);

        for (unsigned i = 0; i < rows_; i++) {
            for (unsigned j = 0; j < cols_; j++) {
                result(i, j) = this(j, i) + rhs(i, j);
            }
        }
        return result;
    }
    MMatrix<T>& operator+=(const MMatrix<T>& rhs) {
        for (unsigned i = 0; i < rhs.rows(); i++) {
            for (unsigned j = 0; j < rhs.cols(); j++) {
                this(i, j) += rhs(i, j);
            }
        }
        return *this;
    }

    // Transpose
    MMatrix<T> transpose() const {
        // Note reversal of rows/cols in constructor args
        MMatrix result(cols_, rows_, 0);

        for (unsigned i = 0; i < rows_; i++) {
            for (unsigned j = 0; j < cols_; j++) {
                result(j, i) = vec[i * cols_ + j];
            }
        }
        return result;
    }

    std::string dim_string() const {
        const MMatrix<T>& lhs = *this;
        std::string lhsdim = std::to_string(lhs.rows()) + "x" + std::to_string(lhs.cols());
        return lhsdim;
    }

    friend std::ostream &operator<<(std::ostream &output, const MMatrix<T> &D ) {
        output << D.dim_string() << std::endl;
        for (unsigned i = 0; i < D.rows_; i++) {
            for (unsigned j = 0; j < D.cols_; j++) {
                output << D.vec[i * D.cols_ + j] << " ";
            }
            output << std::endl;
        }
        return output;
    }
};

#endif  // _MMATRIX_HPP
