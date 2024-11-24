#include <cstdlib>
#include <chrono>
#include "utils.hpp"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

#include <pthread.h>

static const size_t num_items = 50;

struct package_t {
    Complex<double>* inputs;
    int* outputs;
    int n, h, w, tid;
};

void* worker_routine(void* arg) {
    package_t* pkg = (package_t*)arg;
    //pid_t tid = gettid();
    int tid = pkg->tid;
    int w = pkg->w;
    int h = pkg->h;
    int n = pkg->n;
    std::cout << "tid=" << tid << std::endl;
    for (int i = tid * n; i < (tid+1) * n; i++) {
        for (int j = 0; j < w; j++) {
            //std::cout << pkg->inputs[i*w+j] << std::endl;
            pkg->outputs[i*w+j] = mandelbrot_set(pkg->inputs[i*w+j]);
        }
    }
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
    Complex<double>* inputs = (Complex<double>*)malloc(width * height * sizeof(Complex<double>));
    assert(image);
    for (int j = 0; j < height; ++j) {
        double y0 = j * grid_height + lower;
        for (int i = 0; i < width; ++i) {
            double x0 = i * grid_width + left;
            inputs[j*width + i] = Complex<double>(x0, y0);
        }
    }

    auto beg = std::chrono::high_resolution_clock::now();

    /* create thread pool and execute tasks */
    int workers_process_n = height / num_threads;
    int workers_process_r = height % num_threads;
    // TODO: handle remainder
    std::vector<package_t> pkgs(num_threads);
    for (int i = 0; i < num_threads; i++) {
        pkgs[i].inputs = inputs;
        pkgs[i].outputs = image;
        pkgs[i].n = workers_process_n;
        pkgs[i].h = height;
        pkgs[i].w = width;
        pkgs[i].tid = i;
    }
    std::vector<pthread_t> workers(num_threads);
    for (int i = 0; i < num_threads; i++)
        pthread_create(&workers[i], nullptr,
                       worker_routine, &pkgs[i]);

    /* wait for threads */
    for (int i = 0; i < num_threads; i++)
        pthread_join(workers[i], nullptr);

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - beg);
    std::cout << "Elapsed Time = " << duration.count() << "ms" << std::endl;

    /* draw and cleanup */
    write_png(filename, width, height, image);
    free(image);

    return 0;
}