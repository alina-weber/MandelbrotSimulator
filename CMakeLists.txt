cmake_minimum_required(VERSION 3.20)
project(Mandelbrot)

option(USE_OPENCL "Enable OpenCL GPU acceleration" ON)

set(CMAKE_CXX_STANDARD 23)
#set(CMAKE_CXX_COMPILER "/home/weberknecht/LLVM/llvm-project/build-debug/bin/clang++")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -msse4.2 -Wall -Wextra")

# Find SFML
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

if(USE_OPENCL)
    find_package(OpenCL)

    if(OpenCL_FOUND)
        message(STATUS "OpenCL found → GPU enabled")
        set(USE_OPENCL TRUE)
    else()
        message(WARNING "OpenCL not found → GPU disabled")
        set(USE_OPENCL FALSE)
    endif()
endif()

set(SOURCES
        src/main.cpp
        src/mandelbrot.hpp
        src/mandelbrot.cpp
        src/visualizer.cpp
        src/visualizer.hpp
)

if(USE_OPENCL)
    list(APPEND SOURCES src/mandelbrot_gpu.cpp src/mandelbrot_gpu.hpp)
    add_executable(GPU_Test
            src/test_CL.cpp
    )

    # Include directories
    target_include_directories(GPU_Test PRIVATE src include)

    # Link SFML libraries
    target_link_libraries(GPU_Test PRIVATE sfml-graphics sfml-window sfml-system OpenCL::OpenCL )

    add_executable(GPU_Mandelbrot
            src/mandelbrot_gpu.cpp
            src/mandelbrot_gpu.hpp
    )

    # Include directories
    target_include_directories(GPU_Mandelbrot PRIVATE src include)

    # Link SFML libraries
    target_link_libraries(GPU_Mandelbrot PRIVATE sfml-graphics sfml-window sfml-system OpenCL::OpenCL )
endif()
# Add executable
add_executable(Mandelbrot
    ${SOURCES}
)

# Include directories
target_include_directories(Mandelbrot PRIVATE src include)

# Link SFML libraries
if(USE_OPENCL)
    target_link_libraries(Mandelbrot PRIVATE sfml-graphics sfml-window sfml-system OpenCL::OpenCL )
else()
    target_link_libraries(Mandelbrot PRIVATE sfml-graphics sfml-window sfml-system)
endif()

if(USE_OPENCL)
    target_compile_definitions(Mandelbrot PRIVATE USE_OPENCL)
endif()
