#include <iostream>
#include <complex>
#include <vector>
#include <cmath>

using namespace std;

typedef complex<double> Complex;
const double PI = acos(-1);

// Recursive Cooley-Tukey FFT
void fft(vector<Complex> &a) {
    int N = a.size();
    if (N <= 1) return;

    // Divide: separate even and odd indices
    vector<Complex> a_even(N/2), a_odd(N/2);
    for (int i = 0; 2*i < N; i++) {
        a_even[i] = a[2*i];
        a_odd[i] = a[2*i+1];
    }

    // Conquer: FFT on each half
    fft(a_even);
    fft(a_odd);

    // Combine
    for (int k = 0; 2*k < N; k++) {
        Complex t = polar(1.0, -2 * PI * k / N) * a_odd[k];
        a[k] = a_even[k] + t;
        a[k + N/2] = a_even[k] - t;
    }
}

int main() {
    int N = 8;
    vector<Complex> x(N);

    // Example input: sine wave
    for (int i = 0; i < N; i++) {
        x[i] = sin(2 * PI * i / N);
    }

    cout << "Input:\n";
    for (auto val : x) cout << val << endl;

    fft(x);

    cout << "\nFFT Output:\n";
    for (auto val : x) cout << val << endl;

    return 0;
}