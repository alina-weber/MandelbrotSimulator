#include "mandelbrot.hpp"
#include "mandelbrot_gpu.hpp"
#include "visualizer.hpp"
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <chrono>
#include <thread>
#include<fstream>
#include <string>


void calculateAndSaveSimulation(uint16_t * map, double *x_array, double *y_array, double x, double y, double range) {
    //Viewer viewer;
    mandelbrot::calculateMandelbrotThreaded(map, x_array, y_array, x, y, range);
    //printf("Calculation finished\n");
    //visualizeMandelbrotWindow(map, viewer);
    sf::Image image;
    image.create(WIDTH, HEIGHT);
    draw_image(map, image);
    image.saveToFile("mandelbrot.png");
    printf("Image saved\n");
}
void window_visualization(uint16_t * map, double *x_array, double *y_array, double x, double y, double range) {
    Viewer viewer;
    do {
        mandelbrot::calculateMandelbrotThreaded(map, x_array, y_array, x, y, range);
        //printf("Calculation finished\n");
        visualizeMandelbrotWindow(map, viewer);
        if (viewer.event.type == sf::Event::MouseButtonPressed) {
            if (viewer.event.mouseButton.button == sf::Mouse::Left) {
                int mouseX = viewer.event.mouseButton.x;
                int mouseY = viewer.event.mouseButton.y;
                if (mouseX < 0 || mouseX >= WIDTH || mouseY < 0 || mouseY >= HEIGHT) {
                    printf("Error occurred when reading mouse positions.\nMouseX: %d\nMouseY: %d", mouseX, mouseY);
                } else {
                    x = x_array[mouseX];
                    y = y_array[mouseY];
                }
                printf("Range: %f\nX: %f\nY: %f\n", range, x, y);
            }
        } else if (viewer.event.type == sf::Event::MouseWheelScrolled) {
            // negative/down is zooming out
            double x_delta = x_array[viewer.event.mouseWheelScroll.x] - x;
            double y_delta = y_array[viewer.event.mouseWheelScroll.y] - y;
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
}

// Enable or disable non-blocking, raw input mode
void setNonBlocking(bool enable) {
    static struct termios oldt, newt;
    static bool initialized = false;

    if (!initialized) {
        tcgetattr(STDIN_FILENO, &oldt); // save terminal settings
        initialized = true;
    }

    if (enable) {
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO); // disable line buffering and echo
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK); // non-blocking reads
    } else {
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // restore
        fcntl(STDIN_FILENO, F_SETFL, 0);         // blocking reads
    }
}

// Returns -1 if no key pressed
int readKey() {
    char c;
    if (read(STDIN_FILENO, &c, 1) == 1)
        return c;
    return -1;
}

void terminal_visualization(uint16_t * map, double *x_array, double *y_array, double x, double y, double range) {
    int key = 1;
    setNonBlocking(true);
    do{
        if (key != 0) {
            mandelbrot::calculateMandelbrotThreaded(map, x_array, y_array, x, y, range);
            //printf("Calculation finished\n");
            move_curser_to_start();
            visualizeMandelbrotTerminal(map);
        }
        key = readKey();
        if (key == 'x') break;
        switch (key) {
            case 'w': y -= range / (HEIGHT - 1); break;
            case 's': y += range / (HEIGHT - 1); break;
            case 'a': x -= range / (WIDTH - 1); break;
            case 'd': x += range / (WIDTH - 1); break;
            case 'q': range *= 2; break;
            case 'e': range /= 2; break;
            default: key = 0;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 FPS
    } while (true);
}

void calculate_mandelbrot_GPU(uint16_t * map, double *x_array, double *y_array, double x, double y, double range) {
    Viewer viewer;
    do {
    calculateMandelbrotGPU(map, x, y, range, MAX_ITERATIONS, WIDTH, HEIGHT);
        //printf("Calculation finished\n");
        visualizeMandelbrotWindow(map, viewer);
        if (viewer.event.type == sf::Event::MouseButtonPressed) {
            if (viewer.event.mouseButton.button == sf::Mouse::Left) {
                int mouseX = viewer.event.mouseButton.x;
                int mouseY = viewer.event.mouseButton.y;
                if (mouseX < 0 || mouseX >= WIDTH || mouseY < 0 || mouseY >= HEIGHT) {
                    printf("Error occurred when reading mouse positions.\nMouseX: %d\nMouseY: %d", mouseX, mouseY);
                } else {
                    double step = range / (WIDTH - 1);
                    x = x + mouseX * step - range / 2.0;
                    y = y + (mouseY - HEIGHT / 2) * step;
                }
                printf("Range: %f\nX: %f\nY: %f\n", range, x, y);
            }
        } else if (viewer.event.type == sf::Event::MouseWheelScrolled) {
            // negative/down is zooming out
            double step = range / (WIDTH - 1);
            double old_range = range;
            double x_delta = viewer.event.mouseWheelScroll.x * step - range / 2.0;
            //double x_delta = x_array[viewer.event.mouseWheelScroll.x] - x;
            double y_delta = (viewer.event.mouseWheelScroll.y - HEIGHT / 2.0) * step;
            //double y_delta = y_array[viewer.event.mouseWheelScroll.y] - y;
            if (viewer.event.mouseWheelScroll.delta < 0) {
                range = range * 2.0;
                x_delta = x_delta * 2.0;
                y_delta = y_delta * 2.0;
            }
            // positive/up is zooming in
            else if (viewer.event.mouseWheelScroll.delta > 0) {
                range = range / 2.0;
                x_delta = x_delta / 2.0;
                y_delta = y_delta / 2.0;
            }
            // x = x_array[viewer.event.mouseWheelScroll.x] - x_delta;
            x = x + (viewer.event.mouseWheelScroll.x) * step - old_range / 2.0 - x_delta;
            y = y + (viewer.event.mouseWheelScroll.y - HEIGHT / 2.0) * step - y_delta;
        }
    } while (viewer.window.isOpen());
}

int main() {
    auto *map = new uint16_t[WIDTH * HEIGHT];
    double *x_array = new double[WIDTH];
    double *y_array = new double[HEIGHT];
    double x = -0.5;
    double y = 0.0;
    double range = 3.0;
    calculate_mandelbrot_GPU(map, x_array, y_array, x, y, range);
    delete[] map;
    delete[] x_array;
    delete[] y_array;
    return 0;
}
