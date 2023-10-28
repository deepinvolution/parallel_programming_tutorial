if [ -d "./build" ]; then
  rm -r ./build
fi
mkdir build
cd build
CC=mpicc CXX=mpicxx cmake ..
make