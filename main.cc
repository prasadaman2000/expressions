#include <iostream>
#include <string>

#include "runtime.h"
#include "compiler.h"
#include "log.h"

int main(int argc, char ** argv) {
    std::string x = argv[1];

    LOG(0, "Compling: " << x << "\n")

    Compiler c;
    Environment e;
    Program p = c.compile(x, &e);
    std::cout << "expr = " << p.execute(&e) << "\n";
    return 0;
}