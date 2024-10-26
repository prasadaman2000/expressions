#include <iostream>
#include <string>

#include "runtime.h"
#include "compiler.h"

int main() {
    std::string x = "1*pi*2*a";

    Compiler c;
    Environment e;
    Program p = c.compile(x, &e);
    e.add("a", std::make_shared<Constant>(1.0));
    std::cout << "expr = " << p.execute(&e) << "\n";
    std::cout << std::endl;
    return 0;
}