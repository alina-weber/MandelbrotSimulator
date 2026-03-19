//
// Created by weberknecht on 3/10/26.
//
#include "visualizer.hpp"

#define COLOR_STEPS 90


void BG(uint8_t r, uint8_t g, uint8_t b) {
    printf("\x1b[48;2;%d;%d;%dm", r, g, b);
}

void FG(uint8_t r, uint8_t g, uint8_t b) {
    printf("\x1b[38;2;%d;%d;%dm", r, g, b);
}

// void configureColor(size_t color) {
//     if (color == 0) {
//         BG(0, 0, 0);
//         return;
//     }
//     size_t colorSteps = 255 / (MAX_ITERATIONS / 2);
//     if (color > MAX_ITERATIONS / 2) {
//         uint8_t blue = (color - MAX_ITERATIONS / 2) * colorSteps;
//         uint8_t green = 255 - blue;
//         BG(0, green, blue);
//     } else {
//         uint8_t green = color * colorSteps;
//         uint8_t red = 255 - green;
//         BG(red, green, 0);
//     }
// }

void BG_buffered(uint8_t r, uint8_t g, uint8_t b, std::string& buffer){
    buffer += "\x1b[48;2;";
    buffer += std::to_string(r);
    buffer += ";";
    buffer += std::to_string(g);
    buffer += ";";
    buffer += std::to_string(b);
    buffer += "m";
}

void configureColor(size_t color, std::string& buffer) {
    if (color == 0) {
        BG_buffered(0, 0, 0, buffer);
        return;
    }
    size_t colorSteps = 256 / (COLOR_STEPS / 6);
    size_t color_range = (color / (COLOR_STEPS / 6)) % 6 ;
    size_t coloring = color % (COLOR_STEPS / 6);
    // generate rainbow colors
    switch (color_range) {
        //blue to violet
        case 0: {
            uint8_t red = coloring * colorSteps;
            uint8_t blue = 255;
            BG_buffered(red, 0, blue, buffer);
            return;
        }
            // violet to red
        case 1: {
            uint8_t blue = 255 - coloring * colorSteps;
            uint8_t red = 255;
            BG_buffered(red, 0, blue, buffer);
            return;
        }
            // red to yellow
        case 2: {
            uint8_t green = coloring * colorSteps;
            uint8_t red = 255;
            BG_buffered(red, green, 0, buffer);
            return;
        }
            // yellow to green
        case 3: {
            uint8_t green = 255;
            uint8_t red = 255 - coloring * colorSteps;
            BG_buffered(red, green, 0, buffer);
            return;
        }
            // green to turquoise
        case 4: {
            uint8_t green = 255;
            uint8_t blue = coloring * colorSteps;
            BG_buffered(0, green, blue, buffer);
            return;
        }
            // turquoise to blue
        case 5: {
            uint8_t blue = 255;
            uint8_t green = 255 - coloring * colorSteps;
            BG_buffered(0, green, blue, buffer);
            return;
        }
    }
}

// \x1b[A → move cursor up one line
// \x1b[B → move cursor down one line
// \x1b[H → move cursor to home, but don’t clear anything
// \r → move cursor to start of the current line, allows overwriting that line
void move_curser_to_start() {
    printf("\r\x1b[A");
    for (int i = 0; i < HEIGHT; i++) {
        printf("\r\x1b[A");
    }
    fflush(stdout);
}

void visualizeMandelbrotTerminal(uint16_t *map) {
    std::string buffer;
    buffer.reserve(WIDTH * HEIGHT* 16); // avoid reallocations
    for (int x_idx = 0; x_idx < WIDTH; x_idx++) {
        for (int y_idx = 0; y_idx < HEIGHT; y_idx++) {
            configureColor(map[x_idx * HEIGHT + y_idx], buffer);
            buffer += "  ";
        }
        BG_buffered(0, 0, 0, buffer);
        buffer += "\n";
    }
    BG_buffered(0, 0, 0, buffer);
    buffer += "\n";
    printf("%s", buffer.c_str());
    fflush(stdout);
}

sf::Color configureColorInWindow(size_t color) {
    if (color == 0) {
        return sf::Color(0, 0, 0);
    }
    size_t colorSteps = 256 / (COLOR_STEPS / 6);
    size_t color_range = (color / (COLOR_STEPS / 6)) % 6 ;
    size_t coloring = color % (COLOR_STEPS / 6);
    // generate rainbow colors
    switch (color_range) {
        //blue to violet
        case 0: {
            uint8_t red = coloring * colorSteps;
            uint8_t blue = 255;
            return sf::Color(red, 0, blue);
        }
        // violet to red
        case 1: {
            uint8_t blue = 255 - coloring * colorSteps;
            uint8_t red = 255;
            return sf::Color(red, 0, blue);
        }
        // red to yellow
        case 2: {
            uint8_t green = coloring * colorSteps;
            uint8_t red = 255;
            return sf::Color(red, green, 0);
        }
        // yellow to green
        case 3: {
            uint8_t green = 255;
            uint8_t red = 255 - coloring * colorSteps;
            return sf::Color(red, green, 0);
        }
        // green to turquoise
        case 4: {
            uint8_t green = 255;
            uint8_t blue = coloring * colorSteps;
            return sf::Color(0, green, blue);
        }
        // turquoise to blue
        case 5: {
            uint8_t blue = 255;
            uint8_t green = 255 - coloring * colorSteps;
            return sf::Color(0, green, blue);
        }
        default: return sf::Color(0, 0, 0);
    }
}

void draw_image(uint16_t *map, sf::Image& image) {
    for (int y_idx = 0; y_idx < HEIGHT; y_idx++) {
        for (int x_idx = 0; x_idx < WIDTH; x_idx++) {
            image.setPixel(x_idx, y_idx, configureColorInWindow(map[y_idx * WIDTH + x_idx]));
        }
    }
}

void visualizeMandelbrotWindow(uint16_t *map, Viewer &viewer) {
    draw_image(map, viewer.image);
    viewer.texture.loadFromImage(viewer.image);
    viewer.sprite = sf::Sprite(viewer.texture);
    while (viewer.window.isOpen()) {
        sf::Event event;
        while (viewer.window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::Closed: {
                    viewer.window.close();
                    viewer.event = event;
                    break;
                }
                case sf::Event::MouseButtonPressed:
                case sf::Event::MouseWheelScrolled: {
                    viewer.event = event;
                    return;
                }
                default: continue;
            }
        }

        viewer.window.clear();
        viewer.window.draw(viewer.sprite);
        viewer.window.display();
    }
}
