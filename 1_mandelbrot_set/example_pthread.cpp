#include <cstdlib>
#include <chrono>
#include <pthread.h>
#include "utils.hpp"

void* print_thread_id(void* id) {
    long pid = (long)id;
    std::cout << pid << std::endl;
    pthread_exit(NULL);
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

    /* launch threads */
    pthread_t workers[num_threads];
    for(long id = 0; id < num_threads; id++) {
        int ret =  pthread_create(&workers[id], NULL, print_thread_id, (void*)id);
        if(ret != 0) {
            printf("Error: pthread_create() failed\n");
            exit(EXIT_FAILURE);
        }
    }

    /* terminate threads */
    for(long id = 0; id < num_threads; id++) {
        pthread_join(workers[id], NULL);
    }
    return 0;
}