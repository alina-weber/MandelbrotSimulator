#include "mandelbrot.hpp"
#include "visualizer.hpp"

int main() {
  auto * result = new size_t[STEPS * STEPS];
  double * x_array = new double[STEPS];
  double * y_array = new double[STEPS];
  Viewer viewer;
  //printf("Memory was allocated\n");
  double x = 0.0;
  double y = 0.0;
  double range = 4.0;
  do{
    mandelbrot::calculateMandelbrotThreaded(result, x_array, y_array, x, y, range);
    //printf("Calculation complete\n");
    visualizeMandelbrotWindow(result, viewer);
    //printf("Visualization complete\n");
    if (viewer.event.type == sf::Event::MouseButtonPressed) {
      if(viewer.event.mouseButton.button == sf::Mouse::Left) {
        int mouseX = viewer.event.mouseButton.x;
        int mouseY = viewer.event.mouseButton.y;
        if (mouseX < 0  || mouseX >= STEPS || mouseY < 0 || mouseY >= STEPS) {
          printf("Error occurred when reading mouse positions.\nMouseX: %d\nMouseY: %d", mouseX, mouseY);
        } else {
          x = x_array[mouseX];
          y = y_array[mouseY];
        }
      }
    } else if (viewer.event.type == sf::Event::MouseWheelScrolled) {
      // negative/down is zooming out
      double x_delta = x_array[viewer.event.mouseWheelScroll.x] - x ;
      double y_delta = y_array[viewer.event.mouseWheelScroll.y] - y ;
      if (viewer.event.mouseWheelScroll.delta < 0) {
        range = range * 2;
        x_delta = x_delta * 2;
        y_delta = y_delta * 2;
      }
      // positive/up is zooming in
      else if (viewer.event.mouseWheelScroll.delta > 0) {
        range = range / 2;
        x_delta = x_delta / 2;
        y_delta = y_delta / 2;
      }
      x = x_array[viewer.event.mouseWheelScroll.x] - x_delta;
      y = y_array[viewer.event.mouseWheelScroll.y] - y_delta;
    }
} while (viewer.window.isOpen());
  delete[] result;
  delete[] x_array;
  delete[] y_array;
  return 0;
}