#!/bin/sh

num_threads=8
left=-2
right=2
lower=-2
upper=2
width=800
height=800
filename="out.png"

if [ -f "./build/$OUT_FILE" ]; then
  rm "./build/$OUT_FILE"
fi
cd build
./example $num_threads $left $right $lower $upper $width $height $filename
cd ..