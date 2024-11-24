#!/bin/sh

# example="example_omp"
example="example_pthread_no_opt"
# example="example_pthread_thread_pool"
num_threads=2
left=-2
right=2
lower=-2
upper=2
width=400
height=400
filename="out.png"

if [ -f "./build/$OUT_FILE" ]; then
  rm "./build/$OUT_FILE"
fi
cd build
./$example $num_threads $left $right $lower $upper $width $height $filename
cd ..