//
// Created by weberknecht on 3/13/26.
//
#include <CL/cl.h>
#include <iostream>
#include <vector>

int main() {
    cl_uint platformCount = 0;
    clGetPlatformIDs(0, nullptr, &platformCount);

    if (platformCount == 0) {
        std::cout << "No OpenCL platforms found\n";
        return 1;
    }

    std::vector<cl_platform_id> platforms(platformCount);
    clGetPlatformIDs(platformCount, platforms.data(), nullptr);

    for (cl_uint i = 0; i < platformCount; i++) {
        char platformName[256];
        clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, sizeof(platformName), platformName, nullptr);

        std::cout << "Platform " << i << ": " << platformName << "\n";

        cl_uint deviceCount = 0;
        clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, 0, nullptr, &deviceCount);

        std::vector<cl_device_id> devices(deviceCount);
        clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, deviceCount, devices.data(), nullptr);

        for (cl_uint j = 0; j < deviceCount; j++) {
            char deviceName[256];
            clGetDeviceInfo(devices[j], CL_DEVICE_NAME, sizeof(deviceName), deviceName, nullptr);
            cl_ulong mem;
            clGetDeviceInfo(devices[j], CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(mem), &mem, nullptr);
            std::cout << "  Device " << j << ": " << deviceName << "\n";
            std::cout << "Global memory: " << mem / (1024*1024*1024) << " GB\n";
        }
    }

    return 0;
}