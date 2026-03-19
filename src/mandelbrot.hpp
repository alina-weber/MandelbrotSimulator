#include <cstdio>
#include <cstdint>
#include <thread>
#include <CL/cl.h>

// define global parameters for mandelbrot calculation
#define MAX_ITERATIONS 400
#define WIDTH 2000
#define HEIGHT 1300
#define ESCAPE 4.0
#pragma once
namespace mandelbrot {
    class complex {
    private:
        double r;
        double i;
    public:
        complex(double r, double i) {
            this->r = r;
            this->i = i;
        }

        double real() const {
            return r;
        }

        double imag() const {
            return i;
        }

        // Example: addition operator
        complex operator+(const complex& other) const {
            return complex(r + other.r, i + other.i);
        }

        // multiplication of one complex number with another complex number
        // (a + bi) * (c + di) = ac - bd + (ad - bc)i
        complex operator^(const uint32_t exponent) const {
            complex result(1, 0);
            complex base = *this;
            for(uint32_t e = 0; e < exponent; e++){
                const complex tmp = result;
                result.r = tmp.r * base.r - tmp.i * base.i;
                result.i = tmp.r * base.i + tmp.i * base.r;
            }
            return result;
        }

        complex square() const{
            complex result(0,0);
            result.r = this->r * this->r - this->i * this->i;
            result.i = this->r * this->i * 2.0;
            return result;
        }

        static double quadratic_absolute(const complex z){
            return z.r * z.r + z.i * z.i;
        }

        double quadratic_absolute() const{
            return this->r * this->r + this->i * this->i;
        }
    };

    void calculateMandelbrot(uint16_t* map, double * x_array, double * y_array, double x, double y, double range);

    void calculateMandelbrotThreaded(uint16_t* map, double * x_array, double * y_array, double x, double y, double range);
}