#include <iostream>
#define _USE_MATH_DEFINES
#include <cmath>
#include <memory>

#ifdef USE_DOUBLE
using d_f = double;
#else
using d_f = float;
#endif

int main(){
    const int size = 10000000;
    d_f sum = 0.0;
    auto array = std::make_unique<d_f[]>(size);
    for (int i = 0; i < size; i++){
        array[i] = std::sin(i * 2 * M_PI / size);
        sum += array[i];
    }
    std::cout << "Sum:" << sum << std::endl;
    return 0;
}