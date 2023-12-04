#include <iostream>
#include <vector>
#include <algorithm>
#include <string>

#include "utils.hpp"
#include "odd_even_sort.hpp"

int main(int argc, char** argv) {
    // Parse arguments
    if (argc != 4) {
        std::cout << "This program requires three arguments: ";
        std::cout << "n, in_file, and out_file.";
        std::cout << std::endl;
        return -1;
    }
    int N = std::atoi(argv[1]);
    std::string in_file = argv[2];
    std::string out_file = argv[3];

    // Initialize the MPI environment
    MPI_Init(NULL, NULL);
    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Status status;
    if (world_rank == 0) {
        std::cout << "Num data = " << N << std::endl;
        std::cout << "Input file = " << in_file << std::endl;
        std::cout << "Output file = " << out_file << std::endl;
    }

    // Allocate buffers for each processes to sort data
    int buf_element = 0, buf_element_bak;
    std::vector<int> data_buf;
    while (buf_element * world_size < N)
        buf_element += 2;
    buf_element_bak = buf_element;
    if (world_rank == world_size - 1 &&
        buf_element * world_size > N)
        buf_element -= (buf_element * world_size - N);
    data_buf.resize(buf_element);
    
    // Each process performs odd even sort
    read_bin_data(world_size, world_rank, buf_element,
                  in_file, world_rank * buf_element_bak, data_buf);
    odd_even_sort(world_size, world_rank, data_buf);
    write_bin_data(world_size, world_rank, buf_element,
                   out_file, world_rank * buf_element_bak, data_buf);
    MPI_Finalize();
    return 0;
}