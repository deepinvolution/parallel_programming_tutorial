#include <iostream>
#include <vector>
#include <algorithm>

#include "utils.hpp"
#include "odd_even_sort.hpp"

int main(int argc, char** argv) {
    // Initialize the MPI environment
    MPI_Init(NULL, NULL);
    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // Get the name of the processor
    // char processor_name[MPI_MAX_PROCESSOR_NAME];
    // int name_len;
    // MPI_Get_processor_name(processor_name, &name_len);

    // Init and compute data
    int total_elements = 10;
    int q = total_elements / world_size;
    int r = total_elements % world_size;
    int n = q + (world_rank < r);
    std::vector<int> nums(n);
    mpi_init_data(nums);
    odd_even_sort_mpi(nums, q, r);
    MPI_Barrier(MPI_COMM_WORLD);

    // Collect result
    if (world_rank == 0) {
        std::vector<int> res(total_elements);
        MPI_Status status;
        for (int i = 0; i < n; i++)
            res[i] = nums[i];
        auto p = res.data() + n;
        for (int i = 1; i < world_size; i++) {
            if (r == 0 || (r != 0 && i < r)) {
                MPI_Recv(p, n, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
                p += n;
            } else {
                MPI_Recv(p, n - 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
                p += n - 1;
            }
        }
        std::cout << "res:" << std::endl;
        for (int i = 0; i < total_elements; i++) {
            std::cout << res[i] << " ";
        }
        std::cout << std::endl;
    } else {
        MPI_Ssend(nums.data(), n, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    // Finalize the MPI environment.
    MPI_Finalize();

    return 0;
}