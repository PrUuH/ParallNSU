#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <numeric>
#include <fstream>


void parallelMatrixInit(std::vector<std::vector<double>>& matrix, int start, int end) {
    for (int i = start; i < end; ++i) {
        for (int j = start; j < end; ++j) {
            matrix[i][j] = (i == j) ? 2.0 : 1.0;
        }
    }
}

void parallelVectorInit(std::vector<double>& vector, int start, int end) {
    for (int i = start; i < end; ++i) {
        vector[i] = i + 1;
    }
}

void matrixVectorMultiplication(const std::vector<std::vector<double>>& matrix, const std::vector<double>& vector, std::vector<double>& result, int start, int end) {
    for (int i = start; i < end; ++i) {
        result[i] = 0;
        for (int j = start; j < matrix[i].size(); ++j) {
            result[i] += matrix[i][j] * vector[j];
        }
    }
}

int main() {
    std::vector<int> sizes = {20000, 40000};
    std::vector<int> num_threads = {1, 2, 4, 7, 8, 16, 20, 40};

    std::vector<std::vector<double>> runtimes(num_threads.size(), std::vector<double>(sizes.size()));
    std::vector<std::vector<double>> speedups(num_threads.size(), std::vector<double>(sizes.size()));

    std::ofstream outFile("results.csv");
    if (!outFile.is_open()) {
        std::cerr << "error: cant open file" << std::endl;
        return 1;
    }

    outFile << "Matrix Size,Threads,Runtime,SpeedUp\n";

    for (int i = 0; i < num_threads.size(); ++i) {
        int numThreads = num_threads[i];
        for (int j = 0; j < sizes.size(); ++j) {
            int matrixSize = sizes[j];

            //parallel init-on of matrix
            std::vector<std::vector<double>> matrix(matrixSize, std::vector<double>(matrixSize, 1.0));
            std::vector<std::thread> threads;

            int chunkSize = matrixSize / numThreads;
            for (int k = 0; k < numThreads; ++k) {
                threads.emplace_back(parallelMatrixInit, std::ref(matrix), k * chunkSize, (k + 1) * chunkSize);
            }
            for (auto& thread : threads) {
                thread.join();
        }

            //parallel init-on of vector
            std::vector<double> vector(matrixSize, 0.0);
            threads.clear();
            for (int k =0; k < numThreads; ++k) {
                threads.emplace_back(parallelVectorInit, std::ref(vector), k * chunkSize, (k + 1) * chunkSize);
            }
            for (auto& thread : threads){
                thread.join();
            }

            //result init-on
            std::vector<double> result(matrixSize, 0.0);

            auto start_time = std::chrono::high_resolution_clock::now();

            threads.clear();
            for (int k = 0; k < numThreads; ++k) {
                threads.emplace_back(matrixVectorMultiplication, std::ref(matrix), std::ref(vector), std::ref(result), k * chunkSize, (k + 1) * chunkSize);  
            }
            for (auto& thread : threads) {
                thread.join();
            }

            auto end_time = std::chrono::high_resolution_clock::now();
            double runtime = std::chrono::duration<double>(end_time - start_time).count();

            runtimes[i][j] = runtime;
            double speedup = runtimes[0][j] / runtime;
            speedups[i][j] = speedup;

            outFile << matrixSize << "," << numThreads << "," << runtime << "," << speedup << "\n";
        }
    }

    outFile.close();
    std::cout << "Results saved" << std::endl;
    return 0;
}