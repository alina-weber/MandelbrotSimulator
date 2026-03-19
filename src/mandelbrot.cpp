#include "mandelbrot.hpp"
#include <array>

#define WORKERS 10

namespace mandelbrot {
    // z_n+1  = z_n ^ 2 + c
    size_t calculatePixel(double x, double y) {
        complex c(x, y);
        complex z(0, 0);
        size_t counter = 1;

        for (counter = 1; counter < MAX_ITERATIONS; counter++) {
            z = z ^ 2;
            z = z + c;
            if (z.quadratic_absolute() >= ESCAPE * ESCAPE) {
                return counter;
            }
        }
        return 0;
    }

    void calculatePixel_for_threads(uint16_t *map, double *x_vector, double y, size_t y_idx, size_t x_idx) {
        for (size_t i = 0; i < WIDTH/WORKERS && x_idx + i < WIDTH; i++) {
            map[WIDTH * y_idx + x_idx + i] = calculatePixel(x_vector[x_idx + i], y);
        }
    }

    void calculate_x_vector(uint16_t *map, double *x_vector, double y, int y_idx) {
        for (int x_idx = 0; x_idx < WIDTH; x_idx++) {
            map[WIDTH * y_idx + x_idx] = calculatePixel(x_vector[x_idx], y);
        }
    }

    // Thread creation is too expensive for this to be efficient
    void calculate_x_vector_threaded(uint16_t *map, double *x_vector, double y, int y_idx) {
        std::array<std::thread, WORKERS> workers;
        int x_idx = 0;
        if ( WIDTH % WORKERS != 0) {
            printf("STEPS not dividable by WORKERS. Threaded implementation does not support this. Switch to regular implementation.");
            calculate_x_vector(map, x_vector, y, y_idx);
            return;
        }
        while (x_idx < WIDTH ) {
            for (int worker_idx = 0; worker_idx < WORKERS && x_idx < WIDTH; worker_idx++) {
                workers[worker_idx] = std::thread(calculatePixel_for_threads,
                    map, x_vector, y, y_idx, x_idx);
                x_idx += WIDTH / WORKERS;
            }

            for (std::thread &worker: workers) {
                if (worker.joinable())
                worker.join();
            }
        }

    }

    void fill_coordinates(double *x_array, double *y_array, double x, double y, double range) {
        double step = range / (WIDTH - 1);
        // fill coordinates
        for (int i = 0; i < WIDTH; i++) {
            x_array[i] = -(range / 2) + x + step * i;
        }
        for (int i = 0; i < HEIGHT; i++) {
            y_array[i] = - (step * HEIGHT / 2) + y + step * i;
        }
    }

    void calculateMandelbrot(uint16_t *map, double *x_array, double *y_array, double x, double y, double range) {
        fill_coordinates(x_array, y_array, x, y, range);
        for (int y_idx = 0; y_idx < HEIGHT; y_idx++) {
            calculate_x_vector(map, x_array, y_array[y_idx], y_idx);
        }
    }

    void calculateMandelbrotThreaded(uint16_t *map, double *x_array, double *y_array, double x, double y, double range) {
        std::array<std::thread, HEIGHT> workers;

        fill_coordinates(x_array, y_array, x, y, range);
        for (int y_idx = 0; y_idx < HEIGHT; y_idx++) {
            workers[y_idx] = std::thread(calculate_x_vector, map, x_array, y_array[y_idx], y_idx);
        }

        for (std::thread &worker: workers) {
            worker.join();
        }
    }
}
