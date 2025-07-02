// ml_mps.mm
#import <Foundation/Foundation.h>
#import <Metal/Metal.h>
#import <MetalPerformanceShaders/MetalPerformanceShaders.h>
#include <iostream>

// Simple example: C = A x B

int main() {
    // 1. Create Metal device & command queue
    id<MTLDevice> device = MTLCreateSystemDefaultDevice();
    id<MTLCommandQueue> commandQueue = [device newCommandQueue];

    if (!device) {
        std::cerr << "Metal is not supported on this device." << std::endl;
        return -1;
    }

    // Example sizes: A[MxK] * B[KxN] = C[MxN]
    const int M = 2; // rows A
    const int K = 3; // cols A / rows B
    const int N = 4; // cols B

    float A[M * K] = {
        1, 2, 3,
        4, 5, 6
    };
    float B[K * N] = {
        7, 8, 9, 10,
        11,12,13,14,
        15,16,17,18
    };
    float C[M * N] = {0};

    size_t bytesA = sizeof(A);
    size_t bytesB = sizeof(B);
    size_t bytesC = sizeof(C);

    // 2. Create Metal buffers
    id<MTLBuffer> bufferA = [device newBufferWithBytes:A length:bytesA options:MTLResourceStorageModeShared];
    id<MTLBuffer> bufferB = [device newBufferWithBytes:B length:bytesB options:MTLResourceStorageModeShared];
    id<MTLBuffer> bufferC = [device newBufferWithBytes:C length:bytesC options:MTLResourceStorageModeShared];

    // 3. Describe matrices
    MPSMatrixDescriptor *descA = [MPSMatrixDescriptor matrixDescriptorWithRows:M
                                                                        columns:K
                                                                       rowBytes:K * sizeof(float)
                                                                       dataType:MPSDataTypeFloat32];
    MPSMatrixDescriptor *descB = [MPSMatrixDescriptor matrixDescriptorWithRows:K
                                                                        columns:N
                                                                       rowBytes:N * sizeof(float)
                                                                       dataType:MPSDataTypeFloat32];
    MPSMatrixDescriptor *descC = [MPSMatrixDescriptor matrixDescriptorWithRows:M
                                                                        columns:N
                                                                       rowBytes:N * sizeof(float)
                                                                       dataType:MPSDataTypeFloat32];

    MPSMatrix *matrixA = [[MPSMatrix alloc] initWithBuffer:bufferA descriptor:descA];
    MPSMatrix *matrixB = [[MPSMatrix alloc] initWithBuffer:bufferB descriptor:descB];
    MPSMatrix *matrixC = [[MPSMatrix alloc] initWithBuffer:bufferC descriptor:descC];

    // 4. Create MPSMatrixMultiplication
    MPSMatrixMultiplication *matMul =
        [[MPSMatrixMultiplication alloc] initWithDevice:device
                                          transposeLeft:false
                                         transposeRight:false
                                           resultRows:M
                                        resultColumns:N
                                     interiorColumns:K
                                                alpha:1.0
                                                 beta:0.0];

    // 5. Encode command buffer
    id<MTLCommandBuffer> commandBuffer = [commandQueue commandBuffer];

    [matMul encodeToCommandBuffer:commandBuffer
                      leftMatrix:matrixA
                     rightMatrix:matrixB
                    resultMatrix:matrixC];

    [commandBuffer commit];
    [commandBuffer waitUntilCompleted];

    // 6. Read results back to CPU
    memcpy(C, [bufferC contents], bytesC);

    // 7. Print result
    std::cout << \"Result C (\" << M << \" x \" << N << \"):\\n\";
    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < N; ++j) {
            std::cout << C[i * N + j] << \" \";
        }
        std::cout << \"\\n\";
    }

    return 0;
}
