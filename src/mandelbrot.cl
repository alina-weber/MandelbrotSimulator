#pragma OPENCL EXTENSION cl_khr_fp64 : enable

__kernel void mandelbrot(
    __global ushort* output,
    const ushort width,
    const ushort height,
    const double xmin,
    const double ymin,
    const double step,
    const ushort max_iter)
{
    int x_idx = get_global_id(0);
    int y_idx = get_global_id(1);

    int idx = y_idx * width + x_idx;

    double cr = xmin + x_idx * step;
    double ci = ymin + y_idx * step;

    double zr = 0.0;
    double zi = 0.0;

    ushort iter = 0;

    while (zr*zr + zi*zi < 4.0 && iter < max_iter) {

        double tmp = zr*zr - zi*zi + cr;
        zi = 2.0*zr*zi + ci;
        zr = tmp;

        iter++;
    }

    output[idx] = iter % max_iter ;
}