#include "mandelbrot.hpp"
#include <vector>

#define WORKERS 10

namespace mandelbrot {
    // calculate when a complex number escapes
    size_t calculatePixel(double x, double y, uint16_t max_iterations) {
        complex c(x, y);
        complex z(0, 0);
        size_t counter = 1;

        for (counter = 1; counter < max_iterations; counter++) {
            z = z ^ 2;
            z = z + c;
            if (z.quadratic_absolute() >= ESCAPE * ESCAPE) {
                return counter;
            }
        }
        return 0;
    }

    size_t calculate_fatou_and_julia_pixel(double x, double y, uint16_t max_iterations) {
        complex z(x,y);
        // adjusts how julia and fatou set looks like
        complex c(-0.5125,0.5213);
        size_t counter = 1;
        for (counter = 1; counter < max_iterations; counter++) {
            z = z ^ 2;
            z = z + c;
            if (z.quadratic_absolute() >= ESCAPE * ESCAPE) {
                return counter;
            }
        }
        return 0;
    }

    void calculatePixel_for_threads(uint16_t *map, double *x_vector, double y, size_t y_idx,
        size_t x_idx, uint16_t width, uint16_t height, uint16_t max_iterations) {
        for (size_t i = 0; i < width/WORKERS && x_idx + i < width; i++) {
            map[width * y_idx + x_idx + i] = calculatePixel(x_vector[x_idx + i], y, max_iterations);
        }
    }

    // calculates the pixel for one whole row
    void calculate_x_vector(uint16_t *map, double *x_vector, double y, int y_idx, uint16_t width,
        uint16_t height, uint16_t max_iterations) {
        for (int x_idx = 0; x_idx < width; x_idx++) {
            map[width * y_idx + x_idx] = calculatePixel(x_vector[x_idx], y, max_iterations);
        }
    }

    // Thread creation is too expensive for this to be efficient
    // workload is already parallelized as much as possible for CPU with column wise parallelization
    // technically, if all threads have a similar amount of workload,
    // it does not make sense to have more threads than cores
    void calculate_x_vector_threaded(uint16_t *map, double *x_vector, double y, int y_idx,
        uint16_t width, uint16_t height, uint16_t max_iterations) {
        std::array<std::thread, WORKERS> workers;
        int x_idx = 0;
        if ( width % WORKERS != 0) {
            printf("STEPS not dividable by WORKERS. Threaded implementation does not support this. Switch to "
                   "regular implementation.");
            calculate_x_vector(map, x_vector, y, y_idx, width, height, max_iterations);
            return;
        }
        while (x_idx < width ) {
            for (int worker_idx = 0; worker_idx < WORKERS && x_idx < width; worker_idx++) {
                workers[worker_idx] = std::thread(calculatePixel_for_threads,
                    map, x_vector, y, y_idx, x_idx, width, height,max_iterations);
                x_idx += width / WORKERS;
            }

            for (std::thread &worker: workers) {
                if (worker.joinable())
                worker.join();
            }
        }
    }

    // this function fills in all the other complex number coordinates such that the final picture
    // has the color for complex number x + iy in the center of the picture
    void fill_coordinates(double *x_array, double *y_array, double x, double y, double range,
        uint16_t width, uint16_t height) {
        double step = range / (width - 1);
        for (int i = 0; i < width; i++) {
            x_array[i] = -(range / 2) + x + step * i;
        }
        for (int i = 0; i < height; i++) {
            y_array[i] = - (step * height / 2) + y + step * i;
        }
    }

    void calculate_mandelbrot(uint16_t *map, double *x_array, double *y_array, double x, double y,
        double range, uint16_t width, uint16_t height, uint16_t max_iterations) {
        fill_coordinates(x_array, y_array, x, y, range, width, height);
        for (int y_idx = 0; y_idx < height; y_idx++) {
            calculate_x_vector(map, x_array, y_array[y_idx], y_idx, width, height, max_iterations);
        }
    }

    void calculate_mandelbrot_threaded(uint16_t *map, double *x_array, double *y_array, double x,
        double y, double range, uint16_t width, uint16_t height, uint16_t max_iterations) {
        std::vector<std::thread> workers(height);

        fill_coordinates(x_array, y_array, x, y, range, width, height);
        for (int y_idx = 0; y_idx < height; y_idx++) {
            workers[y_idx] = std::thread(calculate_x_vector, map, x_array, y_array[y_idx], y_idx,
                width, height, max_iterations);
        }

        for (std::thread &worker: workers) {
            worker.join();
        }
    }
}
