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

    void calculatePixel_for_threads(size_t *map, double *y_vector, double x, size_t x_idx, size_t y_idx) {
        for (size_t i = 0; i < HEIGHT/WORKERS && y_idx + i < HEIGHT; i++) {
            map[HEIGHT * x_idx + y_idx + i] = calculatePixel(x, y_vector[y_idx + i]);
        }
    }

    void calculate_y_vector(size_t *map, double *y_vector, double x, int x_idx) {
        for (int y_idx = 0; y_idx < HEIGHT; y_idx++) {
            map[HEIGHT * x_idx + y_idx] = calculatePixel(x, y_vector[y_idx]);
        }
    }

    // Thread creation is too expensive for this to be efficient
    void calculate_y_vector_threaded(size_t *map, double *y_vector, double x, int x_idx) {
        std::array<std::thread, WORKERS> workers;
        int y_idx = 0;
        if ( HEIGHT % WORKERS != 0) {
            printf("STEPS not dividable by WORKERS. Threaded implementation does not support this. Switch to regular implementation.");
            calculate_y_vector(map, y_vector, x_idx, x_idx);
            return;
        }
        while (y_idx < HEIGHT ) {
            for (int worker_idx = 0; worker_idx < WORKERS && y_idx < HEIGHT; worker_idx++) {
                workers[worker_idx] = std::thread(calculatePixel_for_threads,
                    map, y_vector, x, x_idx, y_idx);
                y_idx += HEIGHT / WORKERS;
            }

            for (std::thread &worker: workers) {
                if (worker.joinable())
                worker.join();
            }
        }

    }

    void fill_coordinates(double *x_array, double *y_array, double x, double y, double range) {
        double step = range / (HEIGHT - 1);
        // fill coordinates
        for (int i = 0; i < WIDTH; i++) {
            x_array[i] = -(range / 2) + x + step * i;
        }
        for (int i = 0; i < HEIGHT; i++) {
            y_array[i] = -(range / 2) + y + step * i;
        }
    }

    void calculateMandelbrot(size_t *map, double *x_array, double *y_array, double x, double y, double range) {
        fill_coordinates(x_array, y_array, x, y, range);
        for (int x_idx = 0; x_idx < WIDTH; x_idx++) {
            calculate_y_vector(map, y_array, x_array[x_idx], x_idx);
        }
    }

    void calculateMandelbrotThreaded(size_t *map, double *x_array, double *y_array, double x, double y, double range) {
        std::array<std::thread, WIDTH> workers;

        fill_coordinates(x_array, y_array, x, y, range);
        for (int x_idx = 0; x_idx < WIDTH; x_idx++) {
            workers[x_idx] = std::thread(calculate_y_vector, map, y_array, x_array[x_idx], x_idx);
        }

        for (std::thread &worker: workers) {
            worker.join();
        }
    }
}
