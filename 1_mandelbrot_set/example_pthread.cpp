#include <cstdlib>
#include <chrono>
#include "utils.hpp"
#include "pthread_pool.hpp"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

static const size_t num_items = 50;

void* test_func(void *arg)
{
    int *val = (int*)arg;
    int  old = *val;

    *val += 1000;
    printf("tid=%p, old=%d, val=%d\n", pthread_self(), old, *val);

    if (*val%2)
        usleep(100000);

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

    ThreadPool pool(num_threads);
    int* vals = new int[num_items];

    for (int i = 0; i < num_items; i++) {
        vals[i] = i;
        pool.push(test_func, vals+i);
    }

    pool.join();

    delete vals;

    return 0;
}