//
// Created by weberknecht on 3/10/26.
//
#include <cstddef>
#include <SFML/Graphics.hpp>
#include "mandelbrot.hpp"

#ifndef MANDELBROT_VISUALIZER_HPP
#define MANDELBROT_VISUALIZER_HPP

#endif //MANDELBROT_VISUALIZER_HPP

void visualizeMandelbrotTerminal(size_t *);

class Viewer {
public:
    sf::RenderWindow window;
    sf::Image image;
    sf::Texture texture;
    sf::Sprite sprite;
    sf::Event event;

    Viewer() : window(sf::VideoMode(STEPS, STEPS), "Mandelbrot"), event() {
        image.create(STEPS, STEPS);
    }
};

void visualizeMandelbrotWindow(size_t *, Viewer &);
