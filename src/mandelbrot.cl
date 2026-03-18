#pragma OPENCL EXTENSION cl_khr_fp64 : enable

__kernel void mandelbrot(
    __global int* output,
    const int width,
    const int height,
    const double xmin,
    const double ymin,
    const double step,
    const int max_iter)
{
    int x = get_global_id(0);
    int y = get_global_id(1);

    int idx = y * width + x;

    double cr = xmin + x * step;
    double ci = ymin + y * step;

    double zr = 0.0;
    double zi = 0.0;

    int iter = 0;

    while (zr*zr + zi*zi < 4.0 && iter < max_iter) {

        double tmp = zr*zr - zi*zi + cr;
        zi = 2.0*zr*zi + ci;
        zr = tmp;

        iter++;
    }

    output[idx] = iter;
}