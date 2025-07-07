#include <iostream>
#include <complex>
#include <vector>
#include <cmath>
#include <mpi.h>

using namespace std;

typedef complex<double> Complex;
const double PI = acos(-1);

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int N = 8;  // Example size
    vector<Complex> x(N);

    // Rank 0 initializes the input data
    if (rank == 0) {
        for (int i = 0; i < N; i++) {
            x[i] = sin(2 * PI * i / N);
        }
    }

    // Broadcast the input to all processes
    MPI_Bcast(x.data(), 2*N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    // Note: each complex<double> = 2 doubles

    // Each process does its share
    int k_per_proc = N / size;
    int k_start = rank * k_per_proc;
    int k_end = (rank == size - 1) ? N : k_start + k_per_proc;

    vector<Complex> local_X(k_end - k_start);

    for (int k = k_start; k < k_end; ++k) {
        Complex sum = 0;
        for (int n = 0; n < N; ++n) {
            double angle = -2 * PI * k * n / N;
            sum += x[n] * Complex(cos(angle), sin(angle));
        }
        local_X[k - k_start] = sum;
    }

    // Gather results on rank 0
    vector<Complex> X(N);
    vector<int> recvcounts(size);
    vector<int> displs(size);

    for (int i = 0; i < size; ++i) {
        int start = i * k_per_proc;
        int end = (i == size - 1) ? N : start + k_per_proc;
        recvcounts[i] = (end - start) * 2; // complex = 2 doubles
        displs[i] = start * 2;
    }

    MPI_Gatherv(local_X.data(), (k_end - k_start)*2, MPI_DOUBLE,
                X.data(), recvcounts.data(), displs.data(),
                MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        cout << "Input:\n";
        for (auto val : x) cout << val << endl;

        cout << "\nDFT Output:\n";
        for (auto val : X) cout << val << endl;
    }

    MPI_Finalize();
    return 0;
}