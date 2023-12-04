#!/bin/sh

NUM_DATA=371
NUM_PROCESS=4
IN_FILE="in.bin"
OUT_FILE="out.bin"

if [ -f "./build/$IN_FILE" ]; then
  rm "./build/$IN_FILE"
fi
if [ -f "./build/$OUT_FILE" ]; then
  rm "./build/$OUT_FILE"
fi
python3 generate_data.py $NUM_DATA "./build/$IN_FILE"
cd build
mpirun -np $NUM_PROCESS ./example $NUM_DATA $IN_FILE $OUT_FILE
cd ..
python3 examine_data.py $NUM_DATA "./build/$IN_FILE" "./build/$OUT_FILE"