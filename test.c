#include "matrix_multiply.h"
#include <stdio.h>
#include <assert.h>


#define init_grade int g_total = 0,g_fail = 0,g_pass = 0
#define grade(EXPR) ++g_total; if(EXPR) {printf("[PASS] %d\n",g_total);++g_pass;} else {printf("[FAIL] %d\n",g_total);++g_fail;}
#define print_grade printf("DONE! Pass: %d Fail: %d Total: %d\n",g_pass,g_fail,g_total)

init_grade;

void test_math();

int main() {

    test_math(); // math works! Now to break eveyrthing

    matrix a, b, c;

    assert(initialize_matrix(0, 0, NULL) == NULL_PTR);
    assert(initialize_matrix(0, 0, &a) == BAD_DIMENSIONS);
    assert(initialize_matrix(10, 0, &a) == BAD_DIMENSIONS);
    assert(initialize_matrix(10, 10, &a) == OK);

    puts("\n[DONE] INITIALIZE\n");

    // no tests for destroy

    assert(print_matrix(NULL) == NULL_PTR);
    a.rows = 0;
    assert(print_matrix(&a) == BAD_MATRIX);
    a.rows = 10;
    float *backup = a.data;
    a.data = NULL;
    assert(print_matrix(&a) == BAD_MATRIX);
    a.data = backup;
    assert(print_matrix(&a) == OK);

    puts("\n[DONE] PRINT\n");



    initialize_matrix(10, 10, &b);

    // NULL matrix pointers
    grade(matrix_multiply(NULL, NULL, NULL, 0) == NULL_PTR); // 19
    grade(matrix_multiply(&a, NULL, NULL, 0) == NULL_PTR); // 20
    grade(matrix_multiply(&a, &b, NULL, 0) == NULL_PTR); // 21

    // Bad matrix, rows/columns invalid
    a.rows = 0;
    grade(matrix_multiply(&a, &b, &c, 0) == BAD_MATRIX); // 22
    grade(matrix_multiply(&b, &a, &c, 0) == BAD_MATRIX); // 23
    a.rows = 10; a.columns = 0;
    grade(matrix_multiply(&a, &b, &c, 0) == BAD_MATRIX); // 24
    grade(matrix_multiply(&b, &a, &c, 0) == BAD_MATRIX); // 25
    a.columns = 10;

    // bad matrix, data pointer invalid
    backup = a.data; a.data = NULL;
    grade(matrix_multiply(&a, &b, &c, 0) == BAD_MATRIX); // 26
    grade(matrix_multiply(&b, &a, &c, 0) == BAD_MATRIX); // 27
    a.data = backup;

    // Bad dimensions,
    a.columns = 5;
    grade(matrix_multiply(&a, &b, &c, 0) == BAD_DIMENSIONS); // 28
    a.columns = 10; a.rows = 5;
    grade(matrix_multiply(&b, &a, &c, 0) == BAD_DIMENSIONS); // 29
    a.rows = 10;

    // Invalid thread counts
    grade(matrix_multiply(&a, &b, &c, 0) == BAD_THREAD_COUNT); // 30
    grade(matrix_multiply(&a, &b, &c, 120) == BAD_THREAD_COUNT); // 31
    grade(matrix_multiply(&a, &b, &c, 1) == OK); // 32
    grade(print_matrix(&c) == OK); // 33
    putchar('\n');
    destroy_matrix(&c);
    grade(matrix_multiply(&a, &b, &c, 2) == OK); // 34 - even division
    grade(print_matrix(&c) == OK); // 35
    putchar('\n');
    destroy_matrix(&c);
    grade(matrix_multiply(&a, &b, &c, 3) == OK); // 36 - uneven division
    grade(print_matrix(&c) == OK); // 37

    puts("\n[DONE] MULTIPLY\n");

    destroy_matrix(&a);
    destroy_matrix(&b);
    destroy_matrix(&c);

    puts("\n[DONE] DESTROY\n");

    print_grade;

    puts("\nTests Complete");

}

void test_math() {
    matrix a, b, c;
    initialize_matrix(4, 3, &a);
    initialize_matrix(3, 2, &b);

    float a_vals[12] = {6, 6, 6, 3, 7, 2, 8, 9, 2, 3, 10, 3};
    float b_vals[6] = {9, 3, 3, 10, 9, 4};
    float c_vals[8] = {126, 102, 66, 87, 117, 122, 84, 121};

    for (int idx = 0; idx < 12; ++idx) {
        a.data[idx] = a_vals[idx];
    }

    for (int idx = 0; idx < 6; ++idx) {
        b.data[idx] = b_vals[idx];
    }
	
    grade(matrix_multiply(&a, &b, &c, 1) == OK); // 1

    print_matrix(&a);
    putchar('\n');
    print_matrix(&b);
    putchar('\n');
    print_matrix(&c);
    putchar('\n');

    for (int idx = 0; idx < 8; ++idx) {
        grade(c.data[idx] == c_vals[idx]); // 2 - 9
    }

    puts("[DONE] SINGLE THREAD MATH\n");

    grade(matrix_multiply(&a, &b, &c, 3) == OK); // 10

    print_matrix(&c);
    putchar('\n');

    for (int idx = 0; idx < 8; ++idx) {
        grade(c.data[idx] == c_vals[idx]); // 11 - 18
    }

    destroy_matrix(&a);
    destroy_matrix(&b);
    destroy_matrix(&c);

    puts("[DONE] MULTITHREADED MATH\n");
}