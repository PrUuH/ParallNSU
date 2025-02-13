cmake_minimum_required(VERSION 3.10)
project(Sinus)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME} main.cpp)

if(USE_DOUBLE)
    target_compile_definitions(${PROJECT_NAME} PUBLIC -DUSE_DOUBLE=1)
else()
    target_compile_definitions(${PROJECT_NAME} PUBLIC -DUSE_DOUBLE=0)
endif()