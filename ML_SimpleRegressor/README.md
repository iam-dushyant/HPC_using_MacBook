# Code help
This document is created to list useful code to run the above files.

## C++ file with no parallel computing, MPI, openMP, or MPS
`toy.cpp` is a C++ code with no parallel computing or GPU acceleration elements. \\
To compile the code -
```
g++ toy.cpp -o toy
```
To run the executable file -
```
./toy
```

## C++ file with MPI element
Check if MPI compiler wrapper is installed on the local machine.
```
mpicc --version
```
and 
```
which mpicc
```
To compile the C++ code -
```
mpic++ ml_cpu.cpp -o ml_cpu
```
To run the executable -
```
mpirun -np 4 ./ml_cpu
```
where -np is the flag for number of processes

## C++ file with MPS GPU acceleration
The file extension for MPS enablement is `.mm` as opposed to `.cpp`. \\
The header files used for the objective-C++ file is -
```
#import <Metal/Metal.h>
#import <MetalPerformanceShaders/MetalPerformanceShaders.h>
```
To compile the objective-C++ file -
```
clang++ matmul_mps.mm -o matmul_mps -framework Metal -framework MetalPerformanceShaders -framework Foundation
```
To run the executable file -
```
./matmul_mps
```
