#include <iostream>
#include <vector>
#include <chrono>
#include <cmath> 
#include <fstream> 
#include <omp.h>


double integrate_omp(int nsteps) {
    double sum = 0.0;
    double a = -4.0; 
    double b = 4.0;  
    double step = (b - a) / nsteps;

    #pragma omp parallel
    {
        int num_threads = omp_get_num_threads();
        int thread_id = omp_get_thread_num();
        int items_per_thread = nsteps / num_threads;

        int start = thread_id * items_per_thread;
        int end = (thread_id == num_threads - 1) ? nsteps : start + items_per_thread;

        double local_sum = 0.0; 

        for (int i = start; i < end; i++) {
            double x = a + step * (i + 0.5);
            double fx = exp(-x * x);
            local_sum += fx;
        }

        #pragma omp atomic
        sum += local_sum; 
    }

    return sum * step; 
}

int main() {
    int nsteps = 40000000; 

    std::vector<int> num_threads = {1, 2, 4, 7, 8, 16, 20, 40};
    std::vector<double> speedup(num_threads.size());
    std::vector<double> runtimes(num_threads.size());

    double serial_time, parallel_time;

    omp_set_num_threads(1); 
    auto start_time = std::chrono::high_resolution_clock::now();
    double result_serial = integrate_omp(nsteps);
    auto end_time = std::chrono::high_resolution_clock::now();
    serial_time = std::chrono::duration<double>(end_time - start_time).count();

    std::cout << "Time (serial): " << serial_time << " sec" << std::endl;
    std::cout << "Result (serial): " << result_serial << ", Expected: " << sqrt(M_PI) << std::endl << std::endl;

    std::ofstream outputFile("results.csv", std::ios::app);
    if (!outputFile.is_open()) {
        std::cerr << "Unable to open file" << std::endl;
        return 1;
    }

    bool is_file_empty = outputFile.tellp() == 0;
    if (is_file_empty) {
        outputFile << "Threads,Runtime (s),Speedup\n";
    }

    for (int i = 0; i < num_threads.size(); i++) {
        int num_thread = num_threads[i];

        omp_set_num_threads(num_thread); 

        start_time = std::chrono::high_resolution_clock::now();
        double result_parallel = integrate_omp(nsteps);
        end_time = std::chrono::high_resolution_clock::now();
        parallel_time = std::chrono::duration<double>(end_time - start_time).count();

        runtimes[i] = parallel_time;
        speedup[i] = serial_time / parallel_time; 

        std::cout << "Time with " << num_thread << " threads: " << parallel_time << " seconds" << std::endl;
        std::cout << "S with " << num_thread << " threads: " << speedup[i] << std::endl << std::endl;

        outputFile << num_thread << "," << parallel_time << "," << speedup[i] << "\n";
    }

    outputFile.close();

    std::cout << "\nSummary:\n";
    for (int i = 0; i < num_threads.size(); i++) {
        std::cout << num_threads[i] << " threads: S = " << speedup[i] << ", T = " << runtimes[i] << std::endl;
    }

    return 0;
}