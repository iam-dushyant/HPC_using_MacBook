#include <iostream>
#include <complex>
#include <vector>
#include <cmath>

using namespace std;

typedef complex<double> Complex;
const double PI = acos(-1);

// Direct DFT: X[k] = sum_{n=0}^{N-1} x[n] * exp(-2πikn/N)
vector<Complex> dft(const vector<Complex> &x) {
    int N = x.size();
    vector<Complex> X(N);

    for (int k = 0; k < N; ++k) {
        Complex sum = 0;
        for (int n = 0; n < N; ++n) {
            double angle = -2 * PI * k * n / N;
            sum += x[n] * Complex(cos(angle), sin(angle));
        }
        X[k] = sum;
    }

    return X;
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

    vector<Complex> X = dft(x);

    cout << "\nDFT Output:\n";
    for (auto val : X) cout << val << endl;

    return 0;
}