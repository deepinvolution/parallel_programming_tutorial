#include <fstream>
#include <iterator>
#include <vector>
#include <mpi.h>
#include <time.h>

// cpu version read bin data
void read_bin_data(int n, std::string path, std::vector<int>& nums) {
    std::ifstream f(path, std::ios::binary);
    if(f.fail())
        std::cout << "Input file specified doesn't exist." << std::endl;
    for (int i = 0; i < n; i++)
        f.read((char*)&nums[i], sizeof(int));
}

// mpi version read bin data
void read_bin_data(int world_size, int world_rank, int n,
                   std::string path, int offset, std::vector<int>& nums) {
    MPI_File f;
	MPI_File_open(MPI_COMM_WORLD, static_cast<const char*>(path.data()),
                  MPI_MODE_RDONLY, MPI_INFO_NULL, &f);
	MPI_File_read_at(f, sizeof(int) * offset, static_cast<void*>(nums.data()),
                     n, MPI_INT, MPI_STATUS_IGNORE);
	MPI_File_close(&f);
}

// cpu version write bin data
void write_bin_data(int n, std::string path, std::vector<int>& nums) {
    std::ofstream f(path, std::ios::binary);
    if(f.fail())
        std::cout << "Output file specified doesn't exist." << std::endl;
    for (int i = 0; i < n; i++)
        f.write((char*)&nums[i], sizeof(int));
}

// mpi version write bin data
void write_bin_data(int world_size, int world_rank, int n,
                    std::string path, int offset, std::vector<int>& nums) {
    MPI_File f;
	MPI_File_open(MPI_COMM_WORLD, static_cast<const char*>(path.data()),
                  MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &f);
	MPI_File_write_at(f, sizeof(int) * offset, static_cast<void*>(nums.data()),
                      n, MPI_INT, MPI_STATUS_IGNORE);
	MPI_File_close(&f);
}