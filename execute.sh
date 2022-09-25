# make sure this file is executable $ chmod +x ./execute.sh; 

/usr/bin/clang++-11 -fdiagnostics-color=always -g -std=c++20 ./src/main.cpp -o ./bin/main.out;
# OR
# /usr/bin/g++-10 -fdiagnostics-color=always -g -std=c++20 -Werror ./src/main.cpp -o ./bin/main.out;

./bin/main.out