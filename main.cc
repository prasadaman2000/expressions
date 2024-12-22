#include <iostream>
#include <string>

#include "runtime.h"
#include "compiler.h"

int main(int argc, char ** argv) {
    // std::string x = "1*pi*2+a+exp(0)";
    // std::string x = "(x * x / 2) + (x * x * x / 6) + (x * x * x * x / 24) + (x * x * x * x * x / 120)";
    // std::string x = "(2* 2 / 2)";
    std::string x = argv[1];

    Compiler c;
    Environment e;
    Program p = c.compile(x, &e);
    e.add("a", std::make_shared<Constant>(1.0));
    e.add("b", std::make_shared<Constant>(1.0));
    e.add("c", std::make_shared<Constant>(1.0));
    e.add("x", std::make_shared<Constant>(0));
    // e.add("exp", std::make_shared<Constant>(2.0));
    e.dump();
    std::cout << "expr = " << p.execute(&e) << "\n";
    std::cout << std::endl;
    return 0;
}