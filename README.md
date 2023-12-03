# Parallel Programming
This repo show how to use MPI, Pthread, OpenMP, and cuda with a few examples.

## First Example: Odd Even Sort with MPI
### How to run
```console
$ cd 0_odd_even_sort
$ ./install.sh
$ ./run.sh
```
### How to set your own parameters
```console
There are some parameters in 0_odd_even_sort/run.sh
NUM_DATA: total number of data to be sorted
NUM_PROCESS: total number of process to be launched
IN_FILE: input file name
OUT_FILE: output file name
```

## Second Example: Mandelbrot Set with OpenMP
## Third Example: All Pairs Shortest Path with Pthread