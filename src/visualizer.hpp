//
// Created by weberknecht on 3/10/26.
//
#include <cstddef>
#include <SFML/Graphics.hpp>
#include "mandelbrot.hpp"

#ifndef MANDELBROT_VISUALIZER_HPP
#define MANDELBROT_VISUALIZER_HPP

#endif //MANDELBROT_VISUALIZER_HPP

void visualizeMandelbrotTerminal(uint16_t *);

class Viewer {
public:
    sf::RenderWindow window;
    sf::Image image;
    sf::Texture texture;
    sf::Sprite sprite;
    sf::Event event;

    Viewer() : window(sf::VideoMode(WIDTH, HEIGHT), "Mandelbrot"), event() {
        image.create(WIDTH, HEIGHT);
    }
};
void draw_image(uint16_t * map, sf::Image& image);
void move_curser_to_start();
void visualizeMandelbrotWindow(uint16_t *, Viewer &);
