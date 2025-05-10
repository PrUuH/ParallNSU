#include <iostream>
#include <vector>
#include <omp.h>
#include <cmath>
#include <iomanip>
#include <random>
#include <fstream> 


#ifdef NTHREADS
#else
#error "NTHREADS is not defined. Please specified -DNTHREADS=value during compilation."
#endif

#ifdef MATRIX_SIZE
#else
#error "MATRIX_SIZE is not defined. Please specify -DMATRIX_SIZE=value during compilation.(20000x20000 or 40000x40000)"
#endif

const double kITERATION_STEP = 1.0 / 100000.0;
double epsilon = 0.00001;
const int MAX_ITERATIONS = 10000000; 

double CpuSecond() {
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    return ((double)ts.tv_sec + (double)ts.tv_nsec * 1.e-9);
}

void MatrixVectorProductOmp(const long double *matrix, long double *vec, long double *vecRes) {
    #pragma omp parallel for num_threads(NTHREADS) schedule(auto)
    for (size_t i = 0; i < MATRIX_SIZE; i++) {
        vecRes[i] = 0;
        for (size_t j = 0; j < MATRIX_SIZE; j++) {
            vecRes[i] += matrix[i * MATRIX_SIZE + j] * vec[j];
        }
    }
}

void SubtractVecFromVec(long double *vec1, const long double *vec2) {
    #pragma omp parallel for num_threads(NTHREADS) schedule(auto)
    for (size_t i = 0; i < MATRIX_SIZE; i++) {
        vec1[i] -= vec2[i];
    }
}

void MultiplyVecByScalar(long double *vec, const long double &scalar) {
    #pragma omp parallel for num_threads(NTHREADS) schedule(auto)
    for (size_t i = 0; i < MATRIX_SIZE; i++) {
        vec[i] *= scalar;
    }
}

double VecL2Norm(const long double *vec) {
    long double l2Norm = 0.0;
    #pragma omp parallel for num_threads(NTHREADS) schedule(auto) reduction(+:l2Norm)
    for (size_t i = 0; i < MATRIX_SIZE; i++) {
        l2Norm += vec[i] * vec[i];
    }
    return std::sqrt(l2Norm);
}

double IterationMethod() {
    long double* matrixAData = new long double[MATRIX_SIZE * MATRIX_SIZE];
    long double* vecBData = new long double[MATRIX_SIZE];
    long double* vecX = new long double[MATRIX_SIZE];
    long double* vecTemp = new long double[MATRIX_SIZE];

    #pragma omp parallel for num_threads(NTHREADS) schedule(auto)
    for (size_t i = 0; i < MATRIX_SIZE; i++) {
        for (size_t j = 0; j < MATRIX_SIZE; j++) {
            matrixAData[i * MATRIX_SIZE + j] = (i == j) ? 2.0 : 1.0;
        }
    }

    #pragma omp parallel for num_threads(NTHREADS) schedule(auto)
    for (size_t i = 0; i < MATRIX_SIZE; i++) {
        vecBData[i] = MATRIX_SIZE + 1;
        vecX[i] = 0.0;
    }

    const long double* matrixA = matrixAData;
    const long double* vecB = vecBData;

    printf("%f", VecL2Norm(vecB));
    epsilon *= VecL2Norm(vecB);

    int iterationCount = 0;

    double start = CpuSecond();

    while (iterationCount++ >= 0) {
        MatrixVectorProductOmp(matrixA, vecX, vecTemp);
        SubtractVecFromVec(vecTemp, vecB);

        if (VecL2Norm(vecTemp) < epsilon) break;

        if (iterationCount >= MAX_ITERATIONS) {
            std::cerr << "Error: Exceeded maximum number of iterations (" << MAX_ITERATIONS << ")." << std::endl;
            delete[] matrixAData;
            delete[] vecBData;
            delete[] vecX;
            delete[] vecTemp;
            exit(13);
        }

        MultiplyVecByScalar(vecTemp, kITERATION_STEP);
        SubtractVecFromVec(vecX, vecTemp);
    }

    double end = CpuSecond();

    long double sumAbsoluteError = 0.0;
    long double sumRelativeError = 0.0;

    #pragma omp parallel for num_threads(NTHREADS) schedule(auto) reduction(+:sumAbsoluteError, sumRelativeError)
    for (int i = 0; i < MATRIX_SIZE; i++) {
        long double absoluteError = std::abs(vecX[i] - 1.0);
        long double relativeError = std::abs((vecX[i] - 1.0) / 1.0);
        
        sumAbsoluteError += absoluteError;
        sumRelativeError += relativeError;
    }
    

    std::ofstream csvFile("results.csv", std::ios::app); 
    if (!csvFile.is_open()) {
        std::cerr << "Error: Unable to open file for writing." << std::endl;
        delete[] matrixAData;
        delete[] vecBData;
        delete[] vecX;
        delete[] vecTemp;
        exit(1);
    }

    if (csvFile.tellp() == 0) {
        csvFile << "Matrix Size,Threads,Iterations,Time (s),Absolute Error,Relative Error\n";
    }

    csvFile << MATRIX_SIZE << "," << NTHREADS << "," << iterationCount << "," 
            << std::fixed << std::setprecision(4) << (end - start) << "," 
            << sumAbsoluteError << "," << sumRelativeError << "\n";

    csvFile.close();

    delete[] matrixAData;
    delete[] vecBData;
    delete[] vecX;
    delete[] vecTemp;

    return end - start;
}

int main(int argc, char* argv[]) {
    std::cout << "Program using Simple Iteration method for solving linear systems (CLAY)" << std::endl;
    std::cout << "CLAY : A[" << MATRIX_SIZE << "][" << MATRIX_SIZE << "] * x[" << MATRIX_SIZE << "] = b[" << MATRIX_SIZE << "]\n";
    std::cout << "Number of threads: " << NTHREADS << std::endl;
    std::cout << "Memory used: " << static_cast<long double>((MATRIX_SIZE * MATRIX_SIZE + MATRIX_SIZE + MATRIX_SIZE + MATRIX_SIZE) * sizeof(long double)) / (1024 * 1024) << " MiB\n";

    double time = IterationMethod();
    std::cout << "Your calculations took " << std::fixed << std::setprecision(4) << time << " seconds." << std::endl;
    
    return 0;
}