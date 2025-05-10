#include <iostream>
#include <omp.h>
#include <vector>
#include <chrono>
#include <fstream>


void init(std::vector<std::vector<double>>& matrix, std::vector<double>& vector, int matrix_size) {
#pragma omp parallel for
    for (int i = 0; i < matrix_size; i++) {
        for (int j = 0; j < matrix_size; j++)
            matrix[i][j] = i + j;
        vector[i] = i;
    }
}

void multiplication(int num_threads, std::vector<std::vector<double>> matrix, std::vector<double> vector, int threads) {
#pragma omp parallel num_threads(threads)
    {
#pragma omp for schedule(guided)
        for (int i = 0; i < matrix.size(); i++) {
            for (int j = 0; j < matrix.size(); j++) {
                matrix[i][j] *= vector[j];
            }
        }
    }
}

int main() {
    std::vector<int> num_threads = {1, 2, 4, 7, 8, 16, 20, 40};
    std::vector<int> matrix_sizes = {20000, 40000};
    std::vector<std::vector<double>> runtimes(num_threads.size(), std::vector<double>(matrix_sizes.size()));
    std::vector<std::vector<double>> speedups(num_threads.size(), std::vector<double>(matrix_sizes.size()));

    std::ofstream outputFile("results.csv", std::ios::app);

    bool is_file_empty = outputFile.tellp() == 0;
    if (is_file_empty) {
        outputFile << "Threads,Matrix Size,Runtime (s),Speedup\n";
    }

    for (int i = 0; i < num_threads.size(); i++) {
        for (int j = 0; j < matrix_sizes.size(); j++) {
            int threads = num_threads[i];
            int matrix_size = matrix_sizes[j];
            std::vector<std::vector<double>> matrix(matrix_size, std::vector<double>(matrix_size));
            std::vector<double> vector(matrix_size);
            init(matrix, vector, matrix_size);

            auto start_time = std::chrono::high_resolution_clock::now(); 
#pragma omp parallel num_threads(threads)
            {
#pragma omp for schedule(guided)
                for (int q = 0; q < matrix_size; q++)
                    for (int w = 0; w < matrix_size; w++)
                        matrix[q][w] *= vector[w];
            }
            auto end_time = std::chrono::high_resolution_clock::now();
            double runtime = std::chrono::duration<double>(end_time - start_time).count();
            runtimes[i][j] = runtime;

            double speedup;
            if (j == 0) {
                speedup = runtimes[0][0] / runtime; 
            } else {
                speedup = runtimes[0][1] / runtime;
            }
            speedups[i][j] = speedup;

            std::cout << "Runtime with " << threads << " threads and matrix size " << matrix_size << ": " << runtime << " seconds" << std::endl;
            std::cout << "Speedup with " << threads << " threads and matrix size " << matrix_size << ": " << speedup << std::endl << std::endl;

            outputFile << threads << "," << matrix_size << "," << runtime << "," << speedup << "\n";
        }
    }

    outputFile.close();
    std::cout << "Results have been appended to results.csv" << std::endl;
    return 0;
}