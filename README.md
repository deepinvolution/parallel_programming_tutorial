# parallel_programming_tutorial
This repo show how to use MPI, Pthread, OpenMP, and cuda.

## First Example: Odd Even Sort with MPI
### How to run
```console
$ cd mpi_tutorial/odd_even_sort
$ ./install.sh
$ python3 generate_data.py
$ cd build
$ mpirun -np 4 ./build/example
```