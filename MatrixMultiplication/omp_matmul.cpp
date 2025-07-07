#include <iostream> // For input/output operations (e.g., std::cout)
#include <vector>   // For using std::vector to represent matrices
#include <chrono>   // For measuring execution time
#include <omp.h>    // For OpenMP directives and functions

// Define matrix dimensions as constants for easy modification
// For demonstration, keep these relatively small. For larger matrices,
// you'll see more significant performance differences.
const int ROWS_A = 500;    // Number of rows in matrix A
const int COLS_A_ROWS_B = 500; // Number of columns in matrix A, and rows in matrix B
const int COLS_B = 500;    // Number of columns in matrix B

// Function to print a small portion of a matrix for verification
void printMatrixPartial(const std::vector<std::vector<int>>& matrix, int rows, int cols, int print_limit = 5) {
    for (int i = 0; i < std::min(rows, print_limit); ++i) {
        for (int j = 0; j < std::min(cols, print_limit); ++j) {
            std::cout << matrix[i][j] << "\t";
        }
        if (cols > print_limit) {
            std::cout << "...";
        }
        std::cout << std::endl;
    }
    if (rows > print_limit) {
        std::cout << "..." << std::endl;
    }
}

int main() {
    // 1. Matrix Initialization
    // Create matrices A, B, and C (result matrix)
    // Matrices are represented as vectors of vectors for simplicity.
    // In real-world high-performance computing, 1D arrays are often preferred for better cache locality.
    std::vector<std::vector<int>> matrixA(ROWS_A, std::vector<int>(COLS_A_ROWS_B));
    std::vector<std::vector<int>> matrixB(COLS_A_ROWS_B, std::vector<int>(COLS_B));
    std::vector<std::vector<int>> matrixC_sequential(ROWS_A, std::vector<int>(COLS_B, 0));
    std::vector<std::vector<int>> matrixC_parallel(ROWS_A, std::vector<int>(COLS_B, 0));

    // Populate matrixA and matrixB with some values
    // Using simple sequential values for predictability
    for (int i = 0; i < ROWS_A; ++i) {
        for (int j = 0; j < COLS_A_ROWS_B; ++j) {
            matrixA[i][j] = i + j + 1;
        }
    }

    for (int i = 0; i < COLS_A_ROWS_B; ++i) {
        for (int j = 0; j < COLS_B; ++j) {
            matrixB[i][j] = i * j + 2;
        }
    }

    std::cout << "Matrices initialized. Dimensions: A(" << ROWS_A << "x" << COLS_A_ROWS_B
              << "), B(" << COLS_A_ROWS_B << "x" << COLS_B << "), C(" << ROWS_A << "x" << COLS_B << ")" << std::endl;

    // --- Sequential Matrix Multiplication ---
    std::cout << "\n--- Starting Sequential Matrix Multiplication ---" << std::endl;
    auto start_sequential = std::chrono::high_resolution_clock::now();

    // Standard triple-nested loop for matrix multiplication (ijk order)
    // C[i][j] = sum(A[i][k] * B[k][j]) for k from 0 to COLS_A_ROWS_B-1
    for (int i = 0; i < ROWS_A; ++i) {
        for (int j = 0; j < COLS_B; ++j) {
            for (int k = 0; k < COLS_A_ROWS_B; ++k) {
                matrixC_sequential[i][j] += matrixA[i][k] * matrixB[k][j];
            }
        }
    }

    auto end_sequential = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration_sequential = end_sequential - start_sequential;
    std::cout << "Sequential computation finished in: " << duration_sequential.count() << " seconds" << std::endl;

    // --- Parallel Matrix Multiplication with OpenMP ---
    std::cout << "\n--- Starting Parallel Matrix Multiplication with OpenMP ---" << std::endl;

    // Set the number of threads (optional, OpenMP usually defaults to number of cores)
    // omp_set_num_threads(4); // Uncomment to explicitly set thread count

    auto start_parallel = std::chrono::high_resolution_clock::now();

    // The `#pragma omp parallel for` directive tells OpenMP to parallelize the loop.
    // It divides the iterations of the `i` loop among the available threads.
    // Each thread will compute a portion of the rows of matrix C independently.
    //
    // Default data sharing rules:
    // - `i` is implicitly private (loop variable).
    // - `j` and `k` are implicitly private (loop variables within the parallel region).
    // - `matrixA`, `matrixB`, `matrixC_parallel` are implicitly shared (accessed by all threads).
    //   This is safe for `matrixC_parallel` because each thread writes to distinct rows,
    //   avoiding race conditions on the output elements.
    //
    // Loop order (ijk) is generally cache-friendly for C++'s row-major order:
    // - `matrixC_parallel[i][j]` accesses elements contiguously in memory for `j`.
    // - `matrixA[i][k]` accesses elements contiguously in memory for `k`.
    // - `matrixB[k][j]` accesses elements column-wise, which can be a cache bottleneck,
    //   but for large matrices, the `i` loop parallelization often dominates.
    #pragma omp parallel for
    for (int i = 0; i < ROWS_A; ++i) {
        for (int j = 0; j < COLS_B; ++j) {
            for (int k = 0; k < COLS_A_ROWS_B; ++k) {
                matrixC_parallel[i][j] += matrixA[i][k] * matrixB[k][j];
            }
        }
    }

    auto end_parallel = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration_parallel = end_parallel - start_parallel;
    std::cout << "Parallel computation finished in: " << duration_parallel.count() << " seconds" << std::endl;

    // --- Verification (Optional) ---
    // Compare a few elements of the results to ensure correctness
    bool correct = true;
    for (int i = 0; i < ROWS_A; ++i) {
        for (int j = 0; j < COLS_B; ++j) {
            if (matrixC_sequential[i][j] != matrixC_parallel[i][j]) {
                correct = false;
                break;
            }
        }
        if (!correct) break;
    }

    std::cout << "\nVerification: Results are " << (correct ? "identical." : "DIFFERENT - there might be an error!") << std::endl;

    // Print a small portion of the results for visual inspection
    std::cout << "\nPartial Sequential Result (top-left 5x5):" << std::endl;
    printMatrixPartial(matrixC_sequential, ROWS_A, COLS_B);

    std::cout << "\nPartial Parallel Result (top-left 5x5):" << std::endl;
    printMatrixPartial(matrixC_parallel, ROWS_A, COLS_B);

    // --- Performance Analysis ---
    if (duration_parallel.count() > 0) {
        double speedup = duration_sequential.count() / duration_parallel.count();
        std::cout << "\n--- Performance Summary ---" << std::endl;
        std::cout << "Sequential Time: " << duration_sequential.count() << " seconds" << std::endl;
        std::cout << "Parallel Time:   " << duration_parallel.count() << " seconds" << std::endl;
        std::cout << "Speedup: " << speedup << "x" << std::endl;
        std::cout << "Number of threads used (runtime): " << omp_get_max_threads() << std::endl;
        std::cout << "Note: Speedup depends on CPU cores, overheads, and matrix size." << std::endl;
    }

    return 0;
}
