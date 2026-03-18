cmake_minimum_required(VERSION 3.20)
project(Mandelbrot)

set(CMAKE_CXX_STANDARD 23)
#set(CMAKE_CXX_COMPILER "/home/weberknecht/LLVM/llvm-project/build-debug/bin/clang++")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -msse4.2 -Wall -Wextra")

# Find SFML
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

find_package(OpenCL REQUIRED)
# Add executable
add_executable(Mandelbrot
    src/main.cpp
    src/mandelbrot.cpp
    src/visualizer.cpp
    src/visualizer.hpp
)

# Include directories
target_include_directories(Mandelbrot PRIVATE src include)

# Link SFML libraries
target_link_libraries(Mandelbrot PRIVATE sfml-graphics sfml-window sfml-system OpenCL::OpenCL )

add_executable(GPU_Test
        src/test_CL.cpp
)

# Include directories
target_include_directories(GPU_Test PRIVATE src include)

# Link SFML libraries
target_link_libraries(GPU_Test PRIVATE sfml-graphics sfml-window sfml-system OpenCL::OpenCL )

add_executable(GPU_Mandelbrot
        src/test_kernel.cpp
)

# Include directories
target_include_directories(GPU_Mandelbrot PRIVATE src include)

# Link SFML libraries
target_link_libraries(GPU_Mandelbrot PRIVATE sfml-graphics sfml-window sfml-system OpenCL::OpenCL )
