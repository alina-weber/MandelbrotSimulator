#include <stdio.h>
#include <cstdint>
#include <SFML/Graphics.hpp>

#define ESCAPE 100000.0
#define MAX_ITERATIONS 100
#define POWER 2
#define STEPS 1000

struct complex {
  double r;
  double i;
};

// (a + bi) * (c + di) = ac - bd + (ad - bc)i
complex power(complex z){
  complex result = z;
  for(int i = 1; i < POWER; i++){
    complex tmp = result;
    result.r = tmp.r * z.r - tmp.i * z.i;
    result.i = tmp.r * z.i + tmp.i * z.r;
  }
  return result;
}

complex square(complex z){
  complex result;
  result.r = z.r * z.r - z.i * z.i;
  result.i = z.r * z.i * 2.0;
  return result;
}

complex add(complex z, complex c){
  complex result = z;
  result.r += c.r;
  result.i += c.i;
  return result;
}

// ignor root
double qudratic_abs(complex z){
  return z.r * z.r - z.i * z.i;
}

size_t calculatePixel(double x, double y){
  complex c {x, y};
  complex z {0, 0};
  size_t counter = 0;
  
  for(counter = 1; counter < MAX_ITERATIONS; counter++){
    z = power(z);
    z = add(z,c);
    if(qudratic_abs(z) >= ESCAPE * ESCAPE){
      return counter;
    }
  }
  return 0;
}

void calculate_y_vector(size_t* map, double* y_vector, double x, int x_idx){
  for(int y_idx = 0; y_idx < STEPS; y_idx++){
    map[STEPS * x_idx + y_idx] = calculatePixel(x, y_vector[y_idx]);
  }
  return;
}

// the map should be defined as double array of type double and size [range][range]
void calculateMandelbrot(size_t* map, double range){
  double x_array[STEPS];
  double y_array[STEPS];
  double step = range/STEPS;
  for(int i = 0; i < STEPS; i++){
    x_array[i] = -(range/2) + step * i;
    y_array[i] = -(range/2) + step * i;
  }
  for (int x_idx = 0; x_idx < STEPS; x_idx++){
    calculate_y_vector(map, y_array, x_array[x_idx], x_idx);
    //printf("Vector %d done.\n", x_idx);
  }
  return;
}

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

void visualizeMandelbrot(size_t* map){
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



void visualizeMandelbrotInWindow(size_t* map){
  sf::RenderWindow window(sf::VideoMode(STEPS, STEPS), "Mandelbrot");
  sf::Image image;
  image.create(STEPS, STEPS);
  for(int y_idx = 0; y_idx < STEPS; y_idx++){
    for(int x_idx = 0; x_idx < STEPS; x_idx++){
      image.setPixel(x_idx, y_idx, configureColorInWindow(map[x_idx * STEPS + y_idx]));
    }
  }
  
  sf::Texture texture;
  texture.loadFromImage(image);
  sf::Sprite sprite(texture);
  while(window.isOpen()){
    sf::Event event;
    while(window.pollEvent(event)){
      if(event.type == sf::Event::Closed){
        window.close();
      }else {
        printf("Event type: %d\n", event.type);
      }
    }
    window.clear();
    window.draw(sprite);
    window.display();
  }
}



// color is defined by how many iterations are necessary until escape point is reached
// calculate range -2 to 2 for real and imaginary part
int main(){
  size_t result[STEPS][STEPS];
  calculateMandelbrot((size_t *) &result, 3);
  visualizeMandelbrotInWindow((size_t *) &result);
  return 0;
}
