#include <cstdlib>
#include <chrono>
#include <omp.h>
#include "utils.hpp"

int main(int argc, char** argv) {
    /* argument parsing */
    assert(argc == 9);
    int num_threads = strtol(argv[1], 0, 10);
    double left = strtod(argv[2], 0);
    double right = strtod(argv[3], 0);
    double lower = strtod(argv[4], 0);
    double upper = strtod(argv[5], 0);
    int width = strtol(argv[6], 0, 10);
    int height = strtol(argv[7], 0, 10);
    const char* filename = argv[8];
    double grid_width = (right - left) / width;
    double grid_height = (upper - lower) / height;

    omp_set_num_threads(num_threads);
    std::cout << "#thread = " << num_threads << std::endl;

    /* allocate memory for image */
    int* image = (int*)malloc(width * height * sizeof(int));
    assert(image);

    /* mandelbrot set */
    auto beg = std::chrono::high_resolution_clock::now();
    #pragma omp parallel for schedule(dynamic)
    for (int j = 0; j < height; ++j) {
        double y0 = j * grid_height + lower;
        for (int i = 0; i < width; ++i) {
            double x0 = i * grid_width + left;
            image[j * width + i] = mandelbrot_set(Complex<double>{x0, y0});
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - beg);
    std::cout << "Elapsed Time = " << duration.count() << "ms" << std::endl;

    /* draw and cleanup */
    write_png(filename, width, height, image);
    free(image);
}