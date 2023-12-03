#include <fstream>
#include <iterator>
#include <vector>
#include <mpi.h>
#include <time.h>

void read_bin_data(std::string path, std::vector<int>& nums) {
    // Get the file and filesize in bytes
    std::ifstream rf(path, std::ios::binary | std::ios::ate);
    int num_bytes = rf.tellg();
    rf.clear();
    rf.seekg(0, std::ios::beg);

    // read data
    int num_data = num_bytes / 4;
    nums.clear();
    nums.resize(num_data);
    for (int i = 0; i < num_data; i++)
        rf.read((char*)&nums[i], sizeof(int));
}

void mpi_init_data(std::vector<int>& nums) {
    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    srand(time(NULL) + world_rank);
    int n = nums.size();
    for (int i = 0; i < n; i++) nums[i] = rand() % 10;
}