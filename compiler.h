#include "runtime.h"

#include <memory>
#include <string>
#include <map>

class Program {
public:
    Program() = default;
    Program(std::shared_ptr<Expression>);
    float execute(Environment* env);
    std::string dump(Environment* env, Expression *exp);
private:
    std::shared_ptr<Expression> root_;
};

class Compiler {
public:
    Compiler() = default;
    Program compile(std::string, Environment*);
};