// Copyright [2018] <Chris Chisolm>
#include <getopt.h>
#include <Eigen/Dense>
#include <iostream>
#include <vector>
#include <chrono>
#include <mmatrix.hpp>

using Eigen::MatrixXd;
using namespace Eigen;

template <typename T>
void check_ei_mm_size(const MatrixXd& ei, const MMatrix<T>& target) {
    if (ei.rows() != target.rows() || ei.cols() != target.cols()) {
        std::string eidim_string = std::to_string(ei.rows()) + "x" + std::to_string(ei.cols());
        throw std::out_of_range(std::string("Matrix dimensions incorrect for copy source: ") +
            eidim_string + std::string("target dim: ") + target.dim_string());
        return;
    }
}

template <typename T>
void eigen_to_mmatrix(const MatrixXd& ei, MMatrix<T>& target) {
    check_ei_mm_size(ei, target);
    for (int i = 0; i < ei.rows(); i++) {
        for (int j = 0; j < ei.cols(); j++) {
            target(i, j) = ei(i, j);
        }
    }
}

// const double f_epsilon = 0.0001;
const double f_epsilon = 0.005;

inline bool compf(double lhs, double rhs, double epsilon = f_epsilon) {
    // std::cout << lhs << " " << rhs << " " << lhs - rhs << " " << (fabs(lhs - rhs) < epsilon) << std::endl;
    return (fabs(lhs - rhs) < epsilon);
}

template <typename T>
bool equal_eigen_mmatrix(const MatrixXd ei, const MMatrix<T> target) {
    check_ei_mm_size(ei, target);
    for (int i = 0; i < ei.rows(); i++) {
        for (int j = 0; j < ei.cols(); j++) {
            if (compf(target(i, j), ei(i, j)) == false)
                return false;
        }
    }
    return true;
}

template <typename T>
bool equal_mmatrix_mmatrix(const MMatrix<T> lhs, const MMatrix<T> target) {
    for (unsigned i = 0; i < lhs.rows(); i++) {
        for (unsigned j = 0; j < lhs.cols(); j++) {
            if (compf(target(i, j), lhs(i, j)) == false)
                return false;
        }
    }
    return true;
}

inline const char * const BoolToString(bool b) {
  return b ? "true" : "false";
}

struct arg_values {
    bool all_tests = false;
    int block_size = 0;
    bool use_block_size = false;
    bool test_equal = false;
    bool perf_test = false;
    int m_size = 256;
    int n_size = 256;
};

void print_usage() {
    std::cout << "mmult [-aepu] [--block_size <power of 2>] [--m_size <power of 2>] [--m_size <power of 2>]" << std::endl;
}

struct arg_values parse_args(int argc, char **argv) {
    struct arg_values args;

    while (1) {
        int c;
        int option_index = 0;

        static struct option long_options[] = {
            {"all", no_argument, NULL, 'a'},
            {"test_equal", no_argument, NULL, 'e'},
            {"block_size", required_argument, NULL, 'b'},
            {"perf_test", no_argument, NULL, 'p'},
            {"m_size", required_argument, NULL, 'm'},
            {"n_size", required_argument, NULL, 'n'},
            {"usage", no_argument, NULL, 'u'},
            {NULL, 0, NULL, 0}
        };

        c = getopt_long(argc, argv, "ab:epm:n:u", long_options, &option_index);
        if (c == -1)
            break;

        switch (c) {
            case 'a':
               args.all_tests = true;
               break;

            case 'b':
               args.use_block_size = true;
               args.block_size = atoi(optarg);
               break;

            case 'e':
               args.test_equal = true;
               break;

            case 'p':
               args.perf_test = true;
               break;

            case 'm':
               args.m_size = atoi(optarg);
               break;

            case 'u':
                print_usage();
                exit(0);

            case 'n':
               args.n_size = atoi(optarg);
               break;

            case '?':
            default:
               printf("Unknown argument, getopt returned character code: 0%o\n", c);
        }
    }
    return args;
}

int main(int argc, char **argv) {
    struct arg_values args = parse_args(argc, argv);

    if (args.all_tests) {
        MatrixXd m(2, 3);
        m << 1, 2, 3,
        4, 5, 6;
        VectorXd v(3);
        v << 7, 8, 9;
        std::cout << "m * v =" << std::endl << m * v << std::endl;

        MMatrix<double> mm(3, 4);
        std::cout << "declare\n";
        mm = {1.0, 2, 3, 4,
            5, 6, 7, 8,
            9, 10, 11, 12};
        std::cout << "init\n";

        std::cout << mm << std::endl;
        std::cout << "out\n";

        // Initialize from list;
        std::initializer_list<double> i1t6 = {1.0, 2, 3, 4, 5, 6};

        MMatrix<double> mm23(2, 3);
        mm23 = i1t6;
        std::cout << "Initialization list: " << std::endl;
        std::cout << mm23 << std::endl;
        MMatrix<double> mm32(3, 2);
        mm32 = {7, 10,
                8, 11,
                9, 12};
        std::cout << "Temporary initialization list: " << std::endl;
        std::cout << mm32 << std::endl;

        std::cout << "Multiplication 2x3 x 3x2 initialization list: " << std::endl;
        std::cout << mm23 * mm32 << std::endl;
        std::cout << "Transpose:" << std::endl;
        std::cout << (mm23 * mm32).transpose() << std::endl;
        std::cout << "Rect Transpose:" << std::endl;
        std::cout << mm << std::endl;
        std::cout << mm.transpose() << std::endl;
        std::cout << "Transpose equal" << std::endl;
        std::cout << "equal " << BoolToString(equal_mmatrix_mmatrix(mm, (mm.transpose()).transpose())) << std::endl;
        // Should throws_ error
        // std::cout << (*(mm32 * mm32)) << std::endl;
        mm32(1, 1) = 1;
        MMatrix<double> outm = mm23 * mm32;
        std::cout << mm32 << std::endl;
        std::cout << outm << std::endl;

        // Currently allows assignment to temporary matrix
        std::cout << ((mm23 * mm32)(0, 0) = 4) << std::endl;
    }

    float HI = 100;
    float LO = 1;
    int m_size = args.m_size;
    int n_size = args.n_size;

    int loop_count = 10;
    if (m_size >= 2048)
        loop_count = 1;
    if (m_size <= 128)
        loop_count = 100;


    MMatrix<double> mma(m_size, n_size);
    MMatrix<double> mmb(m_size, n_size);
    MMatrix<double> mmc(m_size, n_size);

    MatrixXd ea = LO+(ArrayXXd::Random(m_size, n_size)*0.5+0.5)*(HI-LO);
    eigen_to_mmatrix(ea, mma);
    MatrixXd eb = LO+(ArrayXXd::Random(m_size, n_size)*0.5+0.5)*(HI-LO);
    eigen_to_mmatrix(eb, mmb);

    MatrixXd ec;
    if (args.all_tests || args.test_equal) {
        if (m_size < 15) {
            std::cout << ea * eb << std::endl;
            std::cout << mma * mmb << std::endl;
        }
        // Test equivelent results from eigin library and my library on multiplication
        std::cout << "multiplication with eigin and MMatrix" << std::endl;
        std::cout << "equal " << BoolToString(equal_eigen_mmatrix(ea * eb, mma * mmb)) << std::endl;
    }

    if (args.all_tests || args.perf_test) {
        // Test run annotation
        std::cout << "ikj, ";

        std::cout << "m_size, " << m_size << ", loop_count, " << loop_count << " ";
        {
            std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < loop_count; i++) {
                ec = ea * eb;
            }
            std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
            // std::cout << ec(0, 0) << std::endl;
            // std::cout << "Elapsed hi-res count with eigen matrix: ";
            std::cout << ", eigen, ";
            std::cout << duration / (loop_count);
        }
        {
            std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < loop_count; i++) {
                mmc = mma * mmb;
            }
            std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
            // std::cout << mmc(0, 0) << std::endl;
            std::cout << ", MMatrix, ";
            std::cout << duration / (loop_count);
        }
            std::cout << std::endl;
    }
}

