#include <chrono>
#include <iostream>
#include <string>
#include <fstream>

#include "runtime.h"
#include "compiler.h"
#include "log.h"

int main(int argc, char ** argv) {
    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::milliseconds;

    std::string x = argv[1];

    float lower = std::stof(argv[2]);
    float upper = std::stof(argv[3]);

    LOG(0, "Compling: " << x << "\n")

    Compiler c;
    Environment e;

    auto compile_start = high_resolution_clock::now();
    Program p = c.compile(x, &e);
    auto compile_end = high_resolution_clock::now();
    std::cout << "Compile time: " << duration_cast<milliseconds>(compile_end - compile_start).count() << "ms\n";
    
    std::ofstream file;
    file.open("analysis/data.csv", std::ios::trunc);
    file << "x,y\n";

    auto execute_start = high_resolution_clock::now();
    for(float x_i = lower; x_i <= upper; x_i += 0.01) {
        e.add("x", std::make_shared<Constant>(x_i));
        float answer;
        try {
           answer = p.execute(&e);
        } catch(std::string e) {
            std::cerr << e << '\n';
            return -1;
        }
        
        file << x_i << "," << answer << "\n";
    }
    
    file.close();
    auto execute_end = high_resolution_clock::now();
    
    std::cout << "Run time: " << duration_cast<milliseconds>(execute_end - execute_start).count() << "ms\n";

    e.dump_eval_stats();
    return 0;
}