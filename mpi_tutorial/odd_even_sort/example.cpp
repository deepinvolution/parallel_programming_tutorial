#include <iostream>
#include <vector>
#include <algorithm>

#include "utils.hpp"
#include "odd_even_sort.hpp"

// #define TOTAL_ELEMENTS 10

int main(int argc, char** argv) {
    // Initialize the MPI environment
    MPI_Init(NULL, NULL);
    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Status status;

    // Get the name of the processor
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);
    // std::cout << "Processor name: ";
    // for (int i = 0; i < name_len; i++) std::cout << processor_name[i];
    // std::cout << std::endl;

    // Rank 0 process read input data
    int total_element = 0;
    std::vector<int> data_all;
    if (world_rank == 0) {
        read_bin_data("data.bin", data_all);
        total_element = data_all.size();
        std::cout << "Input data: " << std::endl;
        for (int x : data_all) std::cout << x << " ";
        std::cout << std::endl;
    }
    // MPI_Barrier(MPI_COMM_WORLD);
    MPI_Bcast(&total_element, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Allocate buffers for each processes to sort data
    int buf_element = 0;
    std::vector<int> data_buf;
    while (buf_element * world_size < total_element)
        buf_element += 2;
    if (world_rank == world_size - 1 &&
        buf_element * world_size > total_element)
        buf_element -= (buf_element * world_size - total_element);
    std::cout << "world_rank = " << world_rank
              << ", total_element = " << total_element
              << ", buf_element = " << buf_element << std::endl;
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
    if (world_rank == 0) {
        std::vector<int> data_res(total_element);
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
        std::cout << "Sorted results:" << std::endl;
        for (int i = 0; i < total_element; i++) {
            std::cout << data_res[i] << " ";
        }
        std::cout << std::endl;
    } else {
        if (world_rank == world_size - 1)
            MPI_Ssend(&buf_element, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Ssend(data_buf.data(), buf_element, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }
    
    // Finalize the MPI environment.
    MPI_Finalize();

    return 0;
}