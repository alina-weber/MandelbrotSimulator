#include "mandelbrot.hpp"
#ifdef USE_OPENCL
    #include "mandelbrot_gpu.hpp"
#endif
#include "mandelbrot_gpu.hpp"
#include "visualizer.hpp"
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <chrono>
#include <thread>
#include <fstream>
#include <string>
#include <iostream>


void calculate_and_save_simulation(uint16_t * map, uint16_t width, uint16_t height, uint16_t max_iterations, double x, double y, double range) {
    //Viewer viewer;
    double *x_array = new double[width];
    double *y_array = new double[height];
    mandelbrot::calculate_mandelbrot_threaded(map, x_array, y_array, x, y, range, width, height, max_iterations);
    //printf("Calculation finished\n");
    //visualizeMandelbrotWindow(map, viewer);
    sf::Image image;
    image.create(WIDTH, HEIGHT);
    draw_image(map, image);
    image.saveToFile("mandelbrot.png");
    printf("Image saved\n");
    delete [] x_array;
    delete [] y_array;
}
void window_visualization(uint16_t * map, uint16_t width, uint16_t height, uint16_t max_iterations, double x, double y, double range, bool threaded) {
    Viewer viewer;
    double *x_array = new double[width];
    double *y_array = new double[height];
    do {
        if (threaded) {
            mandelbrot::calculate_mandelbrot_threaded(map, x_array, y_array, x, y, range, width, height, max_iterations);
        } else {
            mandelbrot::calculate_mandelbrot(map, x_array, y_array, x, y, range, width, height, max_iterations);
        }
        //printf("Calculation finished\n");
        visualize_mandelbrot_window(map, viewer);
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
    delete [] x_array;
    delete [] y_array;
}

// Enable or disable non-blocking, raw input mode
void set_non_blocking(bool enable) {
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
int read_key() {
    char c;
    if (read(STDIN_FILENO, &c, 1) == 1)
        return c;
    return -1;
}

void terminal_visualization(uint16_t * map, uint16_t width, uint16_t height, uint16_t max_iterations, double x, double y, double range) {
    int key = 1;
    double *x_array = new double[width];
    double *y_array = new double[height];
    set_non_blocking(true);
    do{
        if (key != 0) {
            mandelbrot::calculate_mandelbrot_threaded(map, x_array, y_array, x, y, range, width, height, max_iterations);
            //printf("Calculation finished\n");
            move_curser_to_start();
            visualize_mandelbrot_terminal(map);
        }
        key = read_key();
        if (key == 'x') break;
        switch (key) {
            case 'w': y -= range / (height - 1); break;
            case 's': y += range / (height - 1); break;
            case 'a': x -= range / (width - 1); break;
            case 'd': x += range / (width - 1); break;
            case 'q': range *= 2; break;
            case 'e': range /= 2; break;
            default: key = 0;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 FPS
    } while (true);
    delete [] x_array;
    delete [] y_array;
}

#ifdef USE_OPENCL
void calculate_mandelbrot_GPU(uint16_t * map, uint16_t width, uint16_t height, uint16_t max_iterations, double x, double y, double range) {
    Viewer viewer;
    do {
    calculate_mandelbrot_GPU(map, x, y, range, max_iterations, width, height);
        visualizeMandelbrotWindow(map, viewer);
        if (viewer.event.type == sf::Event::MouseButtonPressed) {
            if (viewer.event.mouseButton.button == sf::Mouse::Left) {
                int mouseX = viewer.event.mouseButton.x;
                int mouseY = viewer.event.mouseButton.y;
                if (mouseX < 0 || mouseX >= width || mouseY < 0 || mouseY >= height) {
                    printf("Error occurred when reading mouse positions.\nMouseX: %d\nMouseY: %d", mouseX, mouseY);
                } else {
                    double step = range / (width - 1);
                    x = x + mouseX * step - range / 2.0;
                    y = y + (mouseY - height / 2) * step;
                }
                printf("Range: %f\nX: %f\nY: %f\n", range, x, y);
            }
        } else if (viewer.event.type == sf::Event::MouseWheelScrolled) {
            // negative/down is zooming out
            double step = range / (width - 1);
            double old_range = range;
            double x_delta = viewer.event.mouseWheelScroll.x * step - range / 2.0;
            //double x_delta = x_array[viewer.event.mouseWheelScroll.x] - x;
            double y_delta = (viewer.event.mouseWheelScroll.y - height / 2.0) * step;
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
            y = y + (viewer.event.mouseWheelScroll.y - height / 2.0) * step - y_delta;
        }
    } while (viewer.window.isOpen());
}
#else
void calculate_mandelbrot_GPU([[maybe_unused]] uint16_t * map, [[maybe_unused]] uint16_t width,
    [[maybe_unused]] uint16_t height, [[maybe_unused]] uint16_t max_iterations, [[maybe_unused]] double x,
    [[maybe_unused]] double y, [[maybe_unused]] double range) {
    printf("Error: GPU implementation chosen but not configured\n");
}
#endif

enum Mode {
    CPU,
    CPU_THREADED,
    GPU,
    TERMINAL
};

int main(int argc, char* argv[]) {
    uint16_t width = WIDTH;
    uint16_t height = HEIGHT;
    uint16_t max_iterations = MAX_ITERATIONS;
    double range = 3.0;
    double x = -0.5;
    double y = 0.0;
    char* endptr;
    errno = 0;
    Mode mode = CPU_THREADED;
    for (int i = 1; i < argc; i++) {
        endptr = nullptr;
        if (argv[i][0] == '-' && argv[i][2] == '=') {
            switch (argv[i][1]) {
                // configure width
                case 'w': {
                    long arg = std::strtol(argv[i] + 3, &endptr, 10);
                    if (errno || endptr == argv[1] || *endptr != '\0') {
                        break;
                    }
                    if (arg < 0 || arg > 0xffff) {
                        errno = ERANGE;
                    } else {
                        width = arg;
                    }
                    break;
                }
                // configure height
                case 'h': {
                    long arg = std::strtol(argv[i] + 3, &endptr, 10);
                    if (errno || endptr == argv[1] || *endptr != '\0') {
                        break;
                    }
                    if (arg < 0 || arg > 0xffff) {
                        errno = ERANGE;
                    } else {
                        height = arg;
                    }
                    break;
                }
                // configure range
                case 'r': {
                    double arg = std::strtod(argv[i] + 3, &endptr);
                    if (arg <= 0) {
                        errno = ERANGE;
                    } else {
                        range = arg;
                    }
                    break;
                }
                // configure x coordinate of center of calculation
                case 'x': x = std::strtod(argv[i] + 3, &endptr); break;
                // configure y coordinate of center of calculation
                case 'y': y = std::strtod(argv[i] + 3, &endptr); break;
                // configure max iterations before
                case 'i': {
                    long arg = std::strtol(argv[i] + 3, &endptr, 10);
                    if (errno || endptr == argv[1] || *endptr != '\0') {
                        break;
                    }
                    if (arg < 0 || arg > 0xffff) {
                        errno = ERANGE;
                    } else {
                        max_iterations = arg;
                    }
                    break;
                }
                // configure width
                case 'm': {
                    if (std::string(argv[i] + 3) == "CPU") {
                        mode = CPU;
                    } else if (std::string(argv[i] + 3) == "CPU_THREADED") {
                        mode = CPU_THREADED;
                    } else if (std::string(argv[i] + 3) == "GPU") {
                        mode = GPU;
                    } else if (std::string(argv[i] + 3) == "TERMINAL") {
                        mode = TERMINAL;
                    }
                    break;
                }
                default: {
                    std::cout << "Argument is not defined: " << argv[i] << "\n";
                }
            }
            if (endptr == argv[1]) {
                std::cout << "No valid number found while parsing " << argv[i] << "\n";
            }
            else if (errno == ERANGE) {
                std::cout << "Parsed value is out of range.\n" << argv[i] << "\n";
            } else if (*endptr != '\0') {
                std::cout << "Extra characters after argument: " << argv[i] << "\n";
            }
        } else if (argv[i][0] == '-' && argv[i][1] == 'h' && argv[i][2] == '\00') {
            std::cout << "Usage: " << argv[0] << " [options]\n";
            std::cout << "Options:\n";
            std::cout << "\t-w <width>\tSet the width of the window\tuint16_t\n";
            std::cout << "\t-h <height>\tSet the height of the window\tuint16_t\n";
            std::cout << "\t-i <iterations>\tSet the number of iterations\tuint16_t\n";
            std::cout << "\t-r <range>\tSet the range of the window\tdouble\n";
            std::cout << "\t-x <x>\tSet the x coordinate of the window\tdouble\n";
            std::cout << "\t-y <y>\tSet the y coordinate of the window\tdouble\n";
            std::cout << "\t-m <mode>\tSet the calculation mode\t{CPU, CPU_THREADED, GPU, TERMINAL}\n";
            std::cout << "\t-h <help>\tPrint this message\n";
        }
    }
    auto *map = new uint16_t[width * height];
    switch (mode) {
        case CPU: window_visualization(map, width, height, max_iterations, x, y, range, false); break;
        case CPU_THREADED: window_visualization(map, width, height, max_iterations, x, y, range, true); break;
        case GPU: calculate_mandelbrot_GPU(map, width, height, max_iterations, x, y, range); break;
            // Terminal visualization also uses CPU implementation because GPU threading does not make much sense with
            // such little amount of pixels, The bottleneck likely is the print to the terminal anyway
        case TERMINAL: terminal_visualization(map, width, height, max_iterations, x, y, range); break;
    }
    delete[] map;
    return 0;
}
