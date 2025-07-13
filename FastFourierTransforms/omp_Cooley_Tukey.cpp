#include <iostream>
#include <fstream>
#include <complex>
#include <vector>
#include <cmath>
#include <omp.h>

using namespace std;

typedef complex<double> Complex;
const double PI = acos(-1);

// Recursive Cooley–Tukey FFT with OpenMP
void fft(vector<Complex>& a) {
    int N = a.size();
    if (N <= 1) return;

    // Divide: even and odd
    vector<Complex> even(N/2), odd(N/2);
    for (int i = 0; 2*i < N; ++i) {
        even[i] = a[2*i];
        odd[i] = a[2*i + 1];
    }

    // Conquer: parallel recursive calls
    #pragma omp parallel sections
    {
        #pragma omp section
        { fft(even); }

        #pragma omp section
        { fft(odd); }
    }

    // Combine
    #pragma omp parallel for
    for (int k = 0; k < N/2; ++k) {
        Complex t = polar(1.0, -2 * PI * k / N) * odd[k];
        a[k] = even[k] + t;
        a[k + N/2] = even[k] - t;
    }
}

int main() {
    int N = 8;
    vector<Complex> x(N);

    // Example input: sine wave
    for (int i = 0; i < N; ++i) {
        x[i] = sin(2 * PI * i / N);
    }

    cout << "Input:\n";
    for (auto val : x) cout << val << endl;

    fft(x);

    ofstream fout("output_file.txt");
    for (size_t i = 0; i < x.size(); ++i) {
        fout << i << " "
             << real(x[i]) << " "
             << imag(x[i]) << endl;
    }

    fout.close();
    
    cout << "\nFFT Output written to output_file.txt\n";

    return 0;
}