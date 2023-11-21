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

void odd_even_sort_mpi(std::vector<int>& nums, int q, int r) {
    // Get the number of processes
    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // Init array
    int n = nums.size();
    bool is_sorted = false;
    int start_pos, end_pos;
    if (world_rank < r) start_pos = world_rank * n;
    else start_pos = (n + 1) * r + n * (world_rank + 1 - r);
    end_pos = start_pos + n - 1;
    int le = start_pos & 1 ? 1 : 0;
    int re = end_pos & 1 ? n - 1 : n - 2;
    int lo = start_pos & 1 ? 0 : 1;
    int ro = end_pos & 1 ? n - 2 : n - 1;
    while (!is_sorted) {
        // init
        is_sorted = true;
        MPI_Status status;
        int i, rec_data, send_data;

        // comm for even phase
        if (world_rank & 1) {
            if (start_pos & 1) {
                send_data = nums[0];
                MPI_Ssend(&send_data, 1, MPI_INT, world_rank - 1, 0, MPI_COMM_WORLD);
                MPI_Recv(&rec_data, 1, MPI_INT, world_rank - 1, 0, MPI_COMM_WORLD, &status);
            }
        } else {
            if (world_rank + 1 < world_size && (end_pos & 1) == 0) {
                send_data = nums[n - 1];
                MPI_Ssend(&send_data, 1, MPI_INT, world_rank + 1, 0, MPI_COMM_WORLD);
                MPI_Recv(&rec_data, 1, MPI_INT, world_rank + 1, 0, MPI_COMM_WORLD, &status);
            }
        }
        // even phase
        for (i = le; i + 1 <= re; i += 2) {
            if (nums[i] > nums[i + 1]) {
                std::swap(nums[i], nums[i + 1]);
                is_sorted = false;
            }
        }
        if (world_rank & 1) {
            if (start_pos & 1) {
                if (rec_data > nums[0]) {
                    nums[0] = rec_data;
                    is_sorted = false;
                }
            }
        } else {
            if (world_rank + 1 < world_size && (end_pos & 1) == 0) {
                if (nums[n - 1] > rec_data) {
                    nums[n - 1] = rec_data;
                    is_sorted = false;
                }
            }
        }

        // comm for odd phase
        if (world_rank & 1) {
            if ((start_pos & 1) == 0) {
                send_data = nums[0];
                MPI_Ssend(&send_data, 1, MPI_INT, world_rank - 1, 0, MPI_COMM_WORLD);
                MPI_Recv(&rec_data, 1, MPI_INT, world_rank - 1, 0, MPI_COMM_WORLD, &status);
            }
        } else {
            if (world_rank + 1 < world_size && (end_pos & 1)) {
                send_data = nums[n - 1];
                MPI_Ssend(&send_data, 1, MPI_INT, world_rank + 1, 0, MPI_COMM_WORLD);
                MPI_Recv(&rec_data, 1, MPI_INT, world_rank + 1, 0, MPI_COMM_WORLD, &status);
            }
        }
        // odd phase
        for (i = lo; i + 1 <= ro; i += 2) {
            if (nums[i] > nums[i + 1]) {
                std::swap(nums[i], nums[i + 1]);
                is_sorted = false;
            }
        }
        if (world_rank & 1) {
            if ((start_pos & 1) == 0) {
                if (rec_data > nums[0]) {
                    nums[0] = rec_data;
                    is_sorted = false;
                }
            }
        } else {
            if (world_rank + 1 < world_size && (end_pos & 1)) {
                if (nums[n - 1] > rec_data) {
                    nums[n - 1] = rec_data;
                    is_sorted = false;
                }
            }
        }

        // reduce and
        bool b_rec;
        MPI_Allreduce(&is_sorted, &b_rec, 1, MPI_C_BOOL, MPI_LAND, MPI_COMM_WORLD);
        is_sorted = is_sorted && b_rec;
    }
}