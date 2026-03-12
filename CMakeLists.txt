cmake_minimum_required(VERSION 3.20)
project(Mandelbrot)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native -msse4.2 -Wall -Wextra")

# Find SFML
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

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
target_link_libraries(Mandelbrot PRIVATE sfml-graphics sfml-window sfml-system)
