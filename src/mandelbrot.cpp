#include "mandelbrot.hpp"
#include <array>

namespace mandelbrot {
  size_t calculatePixel(double x, double y){
    complex c(x, y);
    complex z(0, 0);
    size_t counter = 1;

    for(counter = 1; counter < MAX_ITERATIONS; counter++){
      z = z ^ 2;
      z = z + c;
      if(z.quadratic_absolute() >= ESCAPE * ESCAPE){
        return counter;
      }
    }
    return 0;
  }

  void calculate_y_vector(size_t* map, double* y_vector, double x, int x_idx){
    for(int y_idx = 0; y_idx < STEPS; y_idx++){
      map[STEPS * x_idx + y_idx] = calculatePixel(x, y_vector[y_idx]);
    }
  }

  // the map should be defined as double array of type double and size [range][range]
  void calculateMandelbrot(size_t* map, double * x_array, double * y_array, double x, double y, double range){
    double step = range/(STEPS-1);
    for(int i = 0; i < STEPS; i++){
      x_array[i] = -(range/2) + x + step * i;
      y_array[i] = -(range/2) + y + step * i;
    }
    for (int x_idx = 0; x_idx < STEPS; x_idx++){
      calculate_y_vector(map, y_array, x_array[x_idx], x_idx);
    }
  }

  // the map should be defined as double array of type double and size [range][range]
  void calculateMandelbrotThreaded(size_t* map, double * x_array, double * y_array, double x, double y, double range){
    double step = range/(STEPS-1);
    std::array<std::thread, STEPS> workers;

    for(int i = 0; i < STEPS; i++){
      x_array[i] = -(range/2) + x + step * i;
      y_array[i] = -(range/2) + y + step * i;
    }
    for (int x_idx = 0; x_idx < STEPS; x_idx++){
      workers[x_idx] = std::thread(calculate_y_vector,map, y_array, x_array[x_idx], x_idx);
    }

    for (std::thread& worker : workers) {
      worker.join();
    }
  }
}
