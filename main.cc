#include <iostream>
#include <string>

#include "runtime.h"
#include "compiler.h"

int main() {
    std::string x = "( 1 + 2 ) * ( ( ( 1 + 2 ) ^ 3 ) + 4 )";

    Compiler c;
    std::shared_ptr<Expression> e = c.compile(x);
    std::cout << "expr = " << e -> evaluate() << "\n";
    std::cout << std::endl;
    return 0;
}