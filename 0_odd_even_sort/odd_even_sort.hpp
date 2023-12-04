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

void odd_even_sort(int world_size, int world_rank, std::vector<int>& nums) {
    // Assume nums is always even size
    // Init array
    MPI_Status status;
    bool is_sorted, is_sorted_all;
    int l, r, rec_data, send_data;
    while (1) {
        // init
        is_sorted = true;
        is_sorted_all = true;

        // even phase (comm before)
        // // no need -> we've forced each section to be even (n % 2 == 0)

        // even phase (sort)
        l = 0, r = nums.size() - 1;
        for (int i = l; i + 1 <= r; i += 2) {
            if (nums[i] > nums[i + 1]) {
                std::swap(nums[i], nums[i + 1]);
                is_sorted = false;
            }
        }

        // even phase (comm after)
        // // no need -> we've forced each section to be even (n % 2 == 0)

        // reduce is_sorted
        MPI_Allreduce(&is_sorted, &is_sorted_all, 1, MPI_C_BOOL, MPI_LAND, MPI_COMM_WORLD);
        if (is_sorted_all) break;

        // odd phase (comm before)
        if (world_rank < world_size - 1) { // sender
            send_data = nums.back();
            MPI_Ssend(&send_data, 1, MPI_INT, world_rank + 1, 0, MPI_COMM_WORLD);
            nums.pop_back();
        }
        if (world_rank > 0) { // receiver
            MPI_Recv(&rec_data, 1, MPI_INT, world_rank - 1, 0, MPI_COMM_WORLD, &status);
            nums.insert(nums.begin(), rec_data);
        }

        // odd phase (sort)
        l = 0, r = nums.size() - 1;
        if (world_rank == 0) l++;
        for (int i = l; i + 1 <= r; i += 2) {
            if (nums[i] > nums[i + 1]) {
                std::swap(nums[i], nums[i + 1]);
                is_sorted = false;
            }
        }

        // odd phase (comm after)
        if (world_rank > 0) { // sender
            send_data = nums.front();
            MPI_Ssend(&send_data, 1, MPI_INT, world_rank - 1, 0, MPI_COMM_WORLD);
            nums.erase(nums.begin());
        }
        if (world_rank < world_size - 1) { // receiver
            MPI_Recv(&rec_data, 1, MPI_INT, world_rank + 1, 0, MPI_COMM_WORLD, &status);
            nums.push_back(rec_data);
        }

        // reduce is_sorted
        MPI_Allreduce(&is_sorted, &is_sorted_all, 1, MPI_C_BOOL, MPI_LAND, MPI_COMM_WORLD);
        if (is_sorted_all) break;
    }
}