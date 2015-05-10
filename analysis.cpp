extern "C" {
#include "matrix_multiply.h"
    // C++ does name mangling to ensure that function names are unique
    // C does not, so we have to tell the compiler that this file is C-style
    // since we're combining objects with our makefile
}
#include <iostream>
#include <chrono>
#include <vector>

using namespace std::chrono;

int main() {

    const unsigned int ITERATIONS = 15;
    std::vector<int> thread_counts = {1, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24};
    matrix a, b, c;
    high_resolution_clock::time_point t1, t2;
    double total_time;

    for (auto thread_no : thread_counts) {
        total_time = 0;
        for (unsigned int itr = 0; itr < ITERATIONS; ++itr) {
            // init
            initialize_matrix(512, 1024, &a);
            initialize_matrix(1024, 512, &b);
            std::cerr << thread_no << '\t' << itr;

            t1 = high_resolution_clock::now();

            // multiply
            matrix_multiply(&a, &b, &c, thread_no);

            t2 = high_resolution_clock::now();

            total_time = ((duration_cast<milliseconds>(t2 - t1)).count() + (itr * total_time)) / (itr + 1);

            std::cerr << '\t' << total_time << std::endl;

            //destroy
            destroy_matrix(&a);
            destroy_matrix(&b);
            destroy_matrix(&c);
        }
        std::cout << thread_no << '\t' << total_time << std::endl;
    }
}
