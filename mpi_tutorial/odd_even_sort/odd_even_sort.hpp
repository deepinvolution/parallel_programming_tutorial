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
    int n = nums.size();
    int l = 0; //n * world_rank;
    int r = n; //l + n;
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
        MPI_Status status;
        int x, y;
        if (world_rank == 1) {
            x = nums[l];
            MPI_Ssend(&x, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
            MPI_Recv(&y, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        } else if (world_rank == 0) {
            y = nums[r - 1];
            MPI_Recv(&x, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, &status);
            MPI_Ssend(&y, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
            // std::cout << "get ->" << x << std::endl;
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
        } else {
            if (y > nums[l]) {
                std::swap(nums[l], y);
                is_sorted = false;
            }
        }
    // if (world_rank == 0) {
    //     std::cout << "mid 0:" << std::endl;
    //     for (int i = 0; i < n; i++) {
    //         std::cout << nums[i] << " ";
    //     }
    //     std::cout << std::endl;
    // }
        bool b_rec;
        MPI_Allreduce(&is_sorted, &b_rec, 1, MPI_C_BOOL, MPI_LAND, MPI_COMM_WORLD);
        is_sorted = is_sorted && b_rec;
    }
}