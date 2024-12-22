#include <chrono>
#include <iostream>
#include <string>

#include "runtime.h"
#include "compiler.h"
#include "log.h"

int main(int argc, char ** argv) {
    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::milliseconds;

    std::string x = argv[1];

    LOG(0, "Compling: " << x << "\n")

    Compiler c;
    Environment e;

    auto compile_start = high_resolution_clock::now();
    Program p = c.compile(x, &e);
    auto compile_end = high_resolution_clock::now();
    std::cout << "Compile time: " << duration_cast<milliseconds>(compile_end - compile_start).count() << "ms\n";

    std::string program_str = p.dump(&e, nullptr);
    std::cout << "compiled program_str = " << program_str << "\n";

    auto execute_start = high_resolution_clock::now();
    float answer = p.execute(&e);
    auto execute_end = high_resolution_clock::now();
    std::cout << "Run time: " << duration_cast<milliseconds>(execute_end - execute_start).count() << "ms\n";

    std::cout << "expr = " << answer << "\n";

    e.dump_eval_stats();
    return 0;
}