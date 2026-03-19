//
// Created by weberknecht on 3/13/26.
//
#include <CL/cl.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

std::string loadKernel(const std::string& path)
{
    std::ifstream file(path);
    return std::string(
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>());
}

int calculateMandelbrotGPU(uint16_t* map, double x, double y, double range, uint16_t max_iter, uint16_t width, uint16_t height)
{

    const double xmin = x - range /2.0;
    const double step = range / (width-1);
    const double ymin = y - step * (height / 2.0);

    // -----------------------------
    // 1. Get OpenCL platform
    // -----------------------------

    cl_uint platformCount;
    clGetPlatformIDs(0, nullptr, &platformCount);

    if(platformCount == 0)
    {
        std::cout << "No OpenCL platform found\n";
        return 1;
    }

    std::vector<cl_platform_id> platforms(platformCount);
    clGetPlatformIDs(platformCount, platforms.data(), nullptr);

    cl_platform_id platform = platforms[0];

    // -----------------------------
    // 2. Get GPU device
    // -----------------------------

    cl_uint deviceCount;
    clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 0, nullptr, &deviceCount);

    if(deviceCount == 0)
    {
        std::cout << "No GPU device found\n";
        return 1;
    }

    std::vector<cl_device_id> devices(deviceCount);
    clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, deviceCount, devices.data(), nullptr);

    cl_device_id device = devices[0];

    char deviceName[256];
    clGetDeviceInfo(device, CL_DEVICE_NAME, sizeof(deviceName), deviceName, nullptr);

    std::cout << "Using GPU: " << deviceName << "\n";

    // -----------------------------
    // 3. Create context + queue
    // -----------------------------

    cl_context context = clCreateContext(nullptr, 1, &device, nullptr, nullptr, nullptr);
    cl_command_queue queue = clCreateCommandQueue(context, device, 0, nullptr);

    // -----------------------------
    // 4. Load kernel
    // -----------------------------

    std::string source = loadKernel("../src/mandelbrot.cl");
    const char* src = source.c_str();
    size_t srcSize = source.size();

    cl_program program = clCreateProgramWithSource(context, 1, &src, &srcSize, nullptr);

    if(clBuildProgram(program, 1, &device, nullptr, nullptr, nullptr) != CL_SUCCESS)
    {
        size_t logSize;
        clGetProgramBuildInfo(program,
                              device,
                              CL_PROGRAM_BUILD_LOG,
                              0,
                              nullptr,
                              &logSize);

        std::vector<char> log(logSize);

        clGetProgramBuildInfo(program,
                              device,
                              CL_PROGRAM_BUILD_LOG,
                              logSize,
                              log.data(),
                              nullptr);

        std::cout << "Build error:\n";
        std::cout << log.data() << std::endl;
        return 1;
    }

    cl_kernel kernel = clCreateKernel(program, "mandelbrot", nullptr);

    // -----------------------------
    // 5. Create output buffer
    // -----------------------------


    cl_mem outputBuffer = clCreateBuffer(
        context,
        CL_MEM_WRITE_ONLY,
        width * height * sizeof(uint16_t),
        nullptr,
        nullptr);

    // -----------------------------
    // 6. Set kernel arguments
    // -----------------------------

    clSetKernelArg(kernel, 0, sizeof(cl_mem), &outputBuffer);
    clSetKernelArg(kernel, 1, sizeof(uint16_t), &width);
    clSetKernelArg(kernel, 2, sizeof(uint16_t), &height);
    clSetKernelArg(kernel, 3, sizeof(double), &xmin);
    clSetKernelArg(kernel, 4, sizeof(double), &ymin);
    clSetKernelArg(kernel, 5, sizeof(double), &step);
    clSetKernelArg(kernel, 6, sizeof(uint16_t), &max_iter);

    // -----------------------------
    // 7. Launch kernel
    // -----------------------------

    size_t globalSize[2] = {(size_t)width, (size_t)height};

    cl_int err = clEnqueueNDRangeKernel(
        queue,
        kernel,
        2,
        nullptr,
        globalSize,
        nullptr,
        0,
        nullptr,
        nullptr);

    if (err != CL_SUCCESS) {
        std::cout << "Kernel launch failed: " << err << "\n";
    }
    clFinish(queue);

    // -----------------------------
    // 8. Read results
    // -----------------------------

    clEnqueueReadBuffer(
        queue,
        outputBuffer,
        CL_TRUE,
        0,
        width * height * sizeof(uint16_t),
        map,
        0,
        nullptr,
        nullptr);
    clFinish(queue);


    // -----------------------------
    // Cleanup
    // -----------------------------

    clReleaseMemObject(outputBuffer);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);

    return 0;
}

// int main() {
//     uint16_t width = 80;
//     uint16_t height = 40;
//     uint16_t max_iter = 100;
//     auto *map = new uint16_t[width * height];
//     double x = -0.5;
//     double y = 0.0;
//     double range = 3.0;
//     calculateMandelbrotGPU(map, x, y, range, max_iter, width, height);
//
//
//     // -----------------------------
//     // 9. ASCII preview
//     // -----------------------------
//
//     for(int y = 0; y < height; y++)
//     {
//         for(int x = 0; x < width; x++)
//         {
//             int v = map[y * width + x];
//
//             if(v == max_iter) std::cout << "#";
//             else if(v > 50) std::cout << "*";
//             else if(v > 10) std::cout << ".";
//             else std::cout << "-";
//         }
//         std::cout << "\n";
//     }
//     delete[] map;
//     return 0;
// }