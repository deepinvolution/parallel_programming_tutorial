#include <vector>
#include <mpi.h>
#include <time.h>

void mpi_init_data(std::vector<int>& nums) {
    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    srand(time(NULL) + world_rank);
    int n = nums.size();
    for (int i = 0; i < n; i++) nums[i] = rand() % 10;
}