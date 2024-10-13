#include "runtime.h"

#include <memory>
#include <string>

class Compiler {
public:
    Compiler() = default;
    std::shared_ptr<Expression> compile(std::string program);
};