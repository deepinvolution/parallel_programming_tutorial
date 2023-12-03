#!/bin/sh

python3 generate_data.py
cd build
mpirun -np 3 ./example