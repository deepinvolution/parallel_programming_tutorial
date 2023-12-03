#include <fstream>
#include <iterator>
#include <vector>
#include <mpi.h>
#include <time.h>

void read_bin_data(int n, std::string path, std::vector<int>& nums) {
    std::ifstream f(path, std::ios::binary);
    if(f.fail())
        std::cout << "Input file specified doesn't exist." << std::endl;
    for (int i = 0; i < n; i++)
        f.read((char*)&nums[i], sizeof(int));
}

void write_bin_data(int n, std::string path, std::vector<int>& nums) {
    std::ofstream f(path, std::ios::binary);
    if(f.fail())
        std::cout << "Output file specified doesn't exist." << std::endl;
    for (int i = 0; i < n; i++)
        f.write((char*)&nums[i], sizeof(int));
}

void mpi_init_data(std::vector<int>& nums) {
    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    srand(time(NULL) + world_rank);
    int n = nums.size();
    for (int i = 0; i < n; i++) nums[i] = rand() % 10;
}