#include <mpi.h>
#include <iostream>
#include <vector>
#include <random>
#include <cmath>

// Synthetic data generation
void generate_data(std::vector<double>& x, std::vector<double>& y, int n, double true_w, double true_b, int seed) {
    std::mt19937 gen(seed);
    std::normal_distribution<> noise(0, 0.1);

    for (int i = 0; i < n; i++) {
        x[i] = i * 0.01;
        y[i] = true_w * x[i] + true_b + noise(gen);
    }
}

// Compute gradients on local data subset
void compute_gradients(const std::vector<double>& x, const std::vector<double>& y, double w, double b,
                       double& grad_w, double& grad_b, int n) {
    grad_w = 0.0;
    grad_b = 0.0;

    for (int i = 0; i < n; i++) {
        double pred = w * x[i] + b;
        double error = pred - y[i];
        grad_w += error * x[i];
        grad_b += error;
    }

    grad_w /= n;
    grad_b /= n;
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int world_rank, world_size;

    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Total dataset size and per-process subset size
    const int total_samples = 1000;
    int local_samples = total_samples / world_size;

    // Allocate local data
    std::vector<double> x(local_samples);
    std::vector<double> y(local_samples);

    // True parameters for synthetic data
    const double true_w = 2.0;
    const double true_b = 1.0;

    // Generate data on root, then scatter to all processes
    if (world_rank == 0) {
        std::vector<double> full_x(total_samples);
        std::vector<double> full_y(total_samples);

        generate_data(full_x, full_y, total_samples, true_w, true_b, 42);

        // Scatter data
        MPI_Scatter(full_x.data(), local_samples, MPI_DOUBLE, x.data(), local_samples, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        MPI_Scatter(full_y.data(), local_samples, MPI_DOUBLE, y.data(), local_samples, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    } else {
        MPI_Scatter(nullptr, local_samples, MPI_DOUBLE, x.data(), local_samples, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        MPI_Scatter(nullptr, local_samples, MPI_DOUBLE, y.data(), local_samples, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    }

    // Initialize model parameters
    double w = 0.0;
    double b = 0.0;

    // Hyperparameters
    const double learning_rate = 0.1;
    const int epochs = 100;

    for (int epoch = 0; epoch < epochs; epoch++) {
        double grad_w_local, grad_b_local;

        // Compute local gradients
        compute_gradients(x, y, w, b, grad_w_local, grad_b_local, local_samples);

        // Reduce gradients across all processes (average)
        double grad_w_global = 0.0;
        double grad_b_global = 0.0;

        MPI_Allreduce(&grad_w_local, &grad_w_global, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
        MPI_Allreduce(&grad_b_local, &grad_b_global, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

        grad_w_global /= world_size;
        grad_b_global /= world_size;

        // Update model parameters (all ranks do the same update)
        w -= learning_rate * grad_w_global;
        b -= learning_rate * grad_b_global;

        if (world_rank == 0 && epoch % 10 == 0) {
            std::cout << "Epoch " << epoch << ": w = " << w << ", b = " << b << std::endl;
        }
    }

    if (world_rank == 0) {
        std::cout << "Training complete. Final parameters: w = " << w << ", b = " << b << std::endl;
    }

    MPI_Finalize();
    return 0;
}