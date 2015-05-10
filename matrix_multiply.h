#ifndef MATRIX_MULTIPLY_H__
#define MATRIX_MULTIPLY_H__

#include <pthread.h>
#include <float.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

typedef struct matrix {
    unsigned int rows, columns;
    float *data; // data stored row-major order
} matrix;

/*
    Transforms 2D index to 1D index

    R - Row number
    C - Column number
    C_LEN - Column size

    Returns 1D index
*/
#define get_index(R,C,C_LEN) ((C_LEN * R) + C)

typedef enum {OK, BAD_DIMENSIONS, BAD_MATRIX, BAD_THREAD_COUNT, NULL_PTR, MEMORY_ERROR, THREAD_ERROR} err_code;
/*
    Error Codes - Refer to output for more details in case of error.

    OK - Operation was successful
    BAD_DIMENSIONS - Matrix dimensions are incompatible
    BAD_MATRIX - Matrix is malformed
    BAD_THREAD_COUNT - Thread count is too high (or zero)
    NULL_PTR - Supplied pointer was null
    MEMORY_ERROR - Internal memory error.
    THREAD_ERROR - Internal threading error.
*/

/*
    Initializes a matrix and fill with random numbers

    rows - number of rows
    columns - number of columns
    new_matrix - pointer to matrix object to initialize

    Returns error code
*/
err_code initialize_matrix(unsigned int rows, unsigned int columns, matrix *new_matrix);

/*
    Destroy a matrix object

    dst_matrix - pointer to matrix to destroy

    No return value
*/
void destroy_matrix(matrix *dst_matrix);

/*
    Prints matrix to stdout (truncated to 3 decimal places)

    mtx - pointer to matrix to print

    Returns error code
*/
err_code print_matrix(matrix *mtx);


/*
    Multiplies two matrices using threads

    lhs - left matrix pointer
    rhs - right matrix pointer
    result - pointer to uninitialized matrix
    threads - number of threads to use

    Result is stored in result matrix.
    Result matrix will only be initialized if the operation was successful
    (no need to destroy on failure)

    Returns error code
*/
err_code matrix_multiply(matrix *lhs, matrix *rhs, matrix *result, const unsigned int threads);
#endif