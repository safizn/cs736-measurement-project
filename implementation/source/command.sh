# g++ defaults are equivalent to running:
gcc a.cpp -xc++ -lstdc++ -shared-libgcc --output a.out

# g++ with C++20
g++ a.cpp -std=c++20 --output a.out

# compile only
g++ -c ./a.cpp ./b.cpp
# Link object files
g++ -o output ./a.o ./b.o
