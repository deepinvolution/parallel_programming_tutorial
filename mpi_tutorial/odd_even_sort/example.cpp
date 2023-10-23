#include <iostream>
#include <vector>
#include <algorithm>
#include <time.h>
#include "odd_even_sort.hpp"

int main(int argc, char** argv) {
    // Initialize the MPI environment
    MPI_Init(NULL, NULL);
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Get the rank of the process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // init vector
    int total_elements = 8;
    int n = total_elements / world_size;
    std::vector<int> nums(n);
    if (world_rank == 0) {
        std::vector<int> total_data(total_elements); 
        srand(time(NULL));
        for (int i = 0; i < n; i++)
            nums[i] = golden[i] = rand() % 10;
        std::sort(golden.begin(), golden.end());
        MPI_Ssend(nums.data(), n, MPI_INT, 1, 0, MPI_COMM_WORLD);
        std::cout << "nums:" << std::endl;
        for (int i = 0; i < n; i++) {
            std::cout << nums[i] << " ";
        }
        std::cout << std::endl;
    } else {
        MPI_Status status;
        MPI_Recv(nums.data(), n, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
    }
    odd_even_sort_mpi(nums);
    if (world_rank == 0) {
        MPI_Status status;
        MPI_Recv(nums.data()+n/2, n/2, MPI_INT, 1, 0, MPI_COMM_WORLD, &status);
    } else {
        MPI_Ssend(nums.data()+n/2, n/2, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    // compare golden
    if (world_rank == 0) {
        std::cout << "nums:" << std::endl;
        for (int i = 0; i < n; i++) {
            std::cout << nums[i] << " ";
        }
        std::cout << std::endl;
        std::cout << "golden:" << std::endl;
        for (int i = 0; i < n; i++) {
            std::cout << golden[i] << " ";
        }
        std::cout << std::endl;
    }

    // Finalize the MPI environment.
    MPI_Finalize();

    return 0;
}