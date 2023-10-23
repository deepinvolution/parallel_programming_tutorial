#include <vector>
#include <algorithm>
#include <mpi.h>

void odd_even_sort(std::vector<int>& nums) {
    int n = nums.size();
    bool is_sorted = false;
    while (!is_sorted) {
        is_sorted = true;
        // even phase
        for (int i = 0; i + 1 < n; i += 2) {
            if (nums[i] > nums[i + 1]) {
                std::swap(nums[i], nums[i + 1]);
                is_sorted = false;
            }
        }
        // odd phase
        for (int i = 1; i + 1 < n; i += 2) {
            if (nums[i] > nums[i + 1]) {
                std::swap(nums[i], nums[i + 1]);
                is_sorted = false;
            }
        }
    }
}

void odd_even_sort_mpi(std::vector<int>& nums) {
    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Get the rank of the process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // Get the name of the processor
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);

    // Init array
    int n = nums.size() / world_size;
    int l = n * world_rank;
    int r = l + n;
    bool is_sorted = false;
    while (!is_sorted) {
        is_sorted = true;
        int i;
        // even phase
        for (i = l; i + 1 < r; i += 2) {
            if (nums[i] > nums[i + 1]) {
                std::swap(nums[i], nums[i + 1]);
                is_sorted = false;
            }
        }
        // comm
        int x;
        if (world_rank == 1) {
            x = nums[l];
            MPI_Ssend(&x, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        } else if (world_rank == 0) {
            MPI_Status status;
            MPI_Recv(&x, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, &status);
        }
        // odd phase
        for (i = l + 1; i + 1 < r; i += 2) {
            if (nums[i] > nums[i + 1]) {
                std::swap(nums[i], nums[i + 1]);
                is_sorted = false;
            }
        }
        if (world_rank == 0) {
            if (nums[i] > x) {
                std::swap(nums[i], x);
                is_sorted = false;
            }
        }
        MPI_Allreduce(&is_sorted, &is_sorted, 1, MPI_C_BOOL, MPI_LAND, MPI_COMM_WORLD);
    }
}