#!/bin/bash

# Compile the C++ code with OpenMP
echo "Step 1/3: Compiling..."
clang++ omp_Cooley_Tukey.cpp -o omp_Cooley_Tukey -fopenmp

# Run it and save output to file
echo "Step 2/3: Writing output..."
./omp_Cooley_Tukey > output_file.txt

echo "Program executed. Output saved to output_file.txt"

# Use gnuplot to plot
echo "Step 3/3: Plotting..."
gnuplot -persist <<-EOFMarker
    set title "FFT Output"
    set xlabel "Index"
    set ylabel "Real Part"
    plot "output_file.txt" using 1:2 with lines title 'Real'
EOFMarker
echo "Plotting complete!"
