//
// Created by weberknecht on 3/10/26.
//
#include "visualizer.hpp"


void BG(uint8_t r, uint8_t g, uint8_t b){
  printf("\x1b[48;2;%d;%d;%dm", r, g, b);
  return;
}

void FG(uint8_t r, uint8_t g, uint8_t b){
  printf("\x1b[38;2;%d;%d;%dm", r, g, b);
  return;
}

void configureColor(size_t color){
  if (color == 0) {
    BG(0,0,0);
    return;
  }
  size_t colorSteps = 255 / (MAX_ITERATIONS/ 2);
  if (color > MAX_ITERATIONS / 2) {
    uint8_t blue = (color - MAX_ITERATIONS/2) * colorSteps;
    uint8_t green = 255 - blue;
    BG(0, green, blue);
    //FG(0, green, blue);
  } else {
    uint8_t green = color * colorSteps;
    uint8_t red = 255 - green;
    BG(red, green, 0);
    //FG(red, green, 0);
  }
}

void visualizeMandelbrotTerminal(size_t* map){
  for(int y_idx = 0; y_idx < STEPS; y_idx++){
    for(int x_idx = 0; x_idx < STEPS; x_idx++){
      configureColor(map[x_idx * STEPS + y_idx]);
      printf("  ");
    }
    BG(0,0,0);
    printf("\n");
  }
  BG(0,0,0);
  printf("\n");
}

sf::Color configureColorInWindow(size_t color){
  if (color == 0) {
    return sf::Color(0,0,0);
  }
  size_t colorSteps = 255 / (MAX_ITERATIONS/ 2);
  if (color > MAX_ITERATIONS / 2) {
    uint8_t blue = (color - MAX_ITERATIONS/2) * colorSteps;
    uint8_t green = 255 - blue;
    return sf::Color(0, green, blue);
    //FG(0, green, blue);
  } else {
    uint8_t green = color * colorSteps;
    uint8_t red = 255 - green;
    return sf::Color(red, green, 0);
    //FG(red, green, 0);
  }
}

void visualizeMandelbrotWindow(size_t* map, Viewer& viewer) {
  for(int y_idx = 0; y_idx < STEPS; y_idx++){
    for(int x_idx = 0; x_idx < STEPS; x_idx++){
      viewer.image.setPixel(x_idx, y_idx, configureColorInWindow(map[x_idx * STEPS + y_idx]));
    }
  }
  viewer.texture.loadFromImage(viewer.image);
  viewer.sprite = sf::Sprite(viewer.texture);
  while(viewer.window.isOpen()){
    sf::Event event;
    while(viewer.window.pollEvent(event)){
      switch(event.type){
        case sf::Event::Closed :{
          viewer.window.close();
          viewer.event = event;
        }
        case sf::Event::MouseButtonPressed:
        case sf::Event::MouseWheelScrolled: {
          viewer.event = event;
          return;
        }
        default: continue;
      }
    }
    // event.type 7 and 8 is scrolling
    // event.type 9 and 10 is clicking
    // event.type 11 is movement of mouse
    //event.type 13 is mouse moving out of frame
    //printf("Event type: %u\n", event.type);

    viewer.window.clear();
    viewer.window.draw(viewer.sprite);
    viewer.window.display();
  }
}