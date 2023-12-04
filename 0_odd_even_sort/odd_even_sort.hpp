#include <vector>
#include <algorithm>
#include <mpi.h>

#define SORT_SUBROUTINE(_nums, _left, _right, _is_sorted) do { \
    for (int i = _left; i + 1 <= _right; i += 2) { \
        if (_nums[i] > _nums[i + 1]) { \
            std::swap(_nums[i], _nums[i + 1]); \
            _is_sorted = false; \
        } \
    } \
} while (0)

void odd_even_sort(std::vector<int>& nums) {
    int n = nums.size();
    bool is_sorted = false;
    while (!is_sorted) {
        is_sorted = true;
        // even phase
        SORT_SUBROUTINE(nums, 0, n - 1, is_sorted);
        // odd phase
        SORT_SUBROUTINE(nums, 1, n - 1, is_sorted);
    }
}

void odd_even_sort(int world_size, int world_rank, std::vector<int>& nums) {
    // Convention (1)
    // each process' nums array is even size except last process.
    // For the simplication of start position of nums
    // regarding original total data array.
    // Convention (2)
    // always send the data to previous process
    // to utilize std::vector push_back() and pop_back()
    
    // Init array
    MPI_Status status;
    bool is_sorted, is_sorted_all;
    int l, r, recv_data, send_data;
    int prev_rank = (world_rank - 1 + world_size) % world_size;
    int next_rank = (world_rank + 1) % world_size;
    int first_rank = 0, last_rank = world_size - 1;

    // even and odd loop
    while (true) {
        // init
        is_sorted = true;
        is_sorted_all = true;

        // even phase (comm before)
        // // no need -> we've forced each section to be even (n % 2 == 0)

        // even phase (sort)
        l = 0, r = nums.size() - 1;
        SORT_SUBROUTINE(nums, l, r, is_sorted);

        // even phase (comm after)
        // // no need -> we've forced each section to be even (n % 2 == 0)

        // reduce is_sorted
        // MPI_Allreduce(&is_sorted, &is_sorted_all, 1, MPI_C_BOOL, MPI_LAND, MPI_COMM_WORLD);
        // if (is_sorted_all) break;

        // odd phase (comm before)
        send_data = nums.front();
        MPI_Sendrecv(&send_data, 1, MPI_INT, prev_rank, 0,
                     &recv_data, 1, MPI_INT, next_rank, 0,
                     MPI_COMM_WORLD, &status);
        nums.push_back(recv_data);

        // odd phase (sort)
        l = 1, r = world_rank == last_rank ?
                   nums.size() - 2 : nums.size() - 1;
        SORT_SUBROUTINE(nums, l, r, is_sorted);

        // odd phase (comm after)
        send_data = nums.back();
        MPI_Sendrecv(&send_data, 1, MPI_INT, next_rank, 0,
                     &recv_data, 1, MPI_INT, prev_rank, 0,
                     MPI_COMM_WORLD, &status);
        if (world_rank != first_rank) nums[0] = recv_data;
        nums.pop_back();

        // reduce is_sorted
        MPI_Allreduce(&is_sorted, &is_sorted_all, 1, MPI_C_BOOL, MPI_LAND, MPI_COMM_WORLD);
        if (is_sorted_all) break;
    }
}