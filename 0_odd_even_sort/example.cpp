#include <iostream>
#include <vector>
#include <algorithm>
#include <string>

#include "utils.hpp"
#include "odd_even_sort.hpp"

int main(int argc, char** argv) {
    // Parse arguments
    if (argc != 4) {
        std::cout << "This program requires three arguments: n, in_file, and out_file." << std::endl;
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

    // Get the name of the processor
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);

    // Rank 0 process read input data
    std::vector<int> data_all;
    if (world_rank == 0) {
        data_all.resize(N);
        read_bin_data(N, in_file, data_all);
    }

    // Allocate buffers for each processes to sort data
    int buf_element = 0;
    std::vector<int> data_buf;
    while (buf_element * world_size < N)
        buf_element += 2;
    if (world_rank == world_size - 1 &&
        buf_element * world_size > N)
        buf_element -= (buf_element * world_size - N);
    data_buf.resize(buf_element);

    // Partition and send data to corresponding process
    if (world_rank == 0) {
        for (int i = 0; i < buf_element; i++)
            data_buf[i] = data_all[i];
        auto p = data_all.data() + buf_element;
        for (int i = 1; i < world_size - 1; i++) {
            MPI_Ssend(p, buf_element, MPI_INT, i, 0, MPI_COMM_WORLD);
            p += buf_element;
        }
        int last_size;
        MPI_Recv(&last_size, 1, MPI_INT, world_size - 1, 0, MPI_COMM_WORLD, &status);
        MPI_Ssend(p, last_size, MPI_INT, world_size - 1, 0, MPI_COMM_WORLD);
    } else {
        if (world_rank == world_size - 1)
            MPI_Ssend(&buf_element, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Recv(data_buf.data(), buf_element, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
    }
    
    // Each process performs odd even sort
    odd_even_sort_mpi(data_buf);
    MPI_Barrier(MPI_COMM_WORLD);

    // Collect result
    std::vector<int> data_res;
    if (world_rank == 0) {
        data_res.resize(N);
        for (int i = 0; i < buf_element; i++)
            data_res[i] = data_buf[i];

        auto p = data_res.data() + buf_element;
        for (int i = 1; i < world_size - 1; i++) {
            MPI_Recv(p, buf_element, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
            p += buf_element;
        }
        int last_size = 0;
        MPI_Recv(&last_size, 1, MPI_INT, world_size - 1, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(p, last_size, MPI_INT, world_size - 1, 0, MPI_COMM_WORLD, &status);
    } else {
        if (world_rank == world_size - 1)
            MPI_Ssend(&buf_element, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Ssend(data_buf.data(), buf_element, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    // Rank 0 process write output data
    if (world_rank == 0) {
        write_bin_data(N, out_file, data_res);
    }
    
    // Finalize the MPI environment.
    MPI_Finalize();

    return 0;
}