#include <cstdlib>
#include <chrono>
#include "utils.hpp"
#include "thread_pool.hpp"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

static const size_t num_items = 50;

struct package_t {
    std::vector<Complex<double>> inputs;
    std::vector<int> outputs;
};

void* wrapper_function(void* arg) {
    package_t* pkg = (package_t*)arg;
    int n = pkg->inputs.size();
    for (int i = 0; i < n; i++)
        pkg->outputs[i] = mandelbrot_set(pkg->inputs[i]);
    return nullptr;
}

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

    /* allocate memory for image */
    int* image = (int*)malloc(width * height * sizeof(int));
    assert(image);

auto beg = std::chrono::high_resolution_clock::now();

    /* create thread pool and execute tasks */
    std::vector<package_t> pkgs(height);
    for (int i = 0; i < height; i++) {
        pkgs[i].inputs.resize(width);
        pkgs[i].outputs.resize(width);
    }
    ThreadPool pool(num_threads);
    for (int j = 0; j < height; ++j) {
        double y0 = j * grid_height + lower;
        for (int i = 0; i < width; ++i) {
            double x0 = i * grid_width + left;
            pkgs[j].inputs[i] = Complex<double>(x0, y0);
        }
        pool.push(wrapper_function, &pkgs[j]);
    }

    /* wait for threads */
    pool.join();
    for (int j = 0; j < height; ++j)
        for (int i = 0; i < width; ++i)
            image[j * width + i] = pkgs[j].outputs[i];

auto end = std::chrono::high_resolution_clock::now();
auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - beg);
std::cout << "Elapsed Time = " << duration.count() << "ms" << std::endl;

    /* draw and cleanup */
    write_png(filename, width, height, image);
    free(image);

    return 0;
}