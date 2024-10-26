#ifndef RUNTIME_H
#define RUNTIME_H

#include <functional>
#include <memory>
#include <map>
#include <iostream>
#include <cmath>
#include <set>

enum class ExpressionType {Base, Constant, Operation, Variable};

class Environment;

class Expression {
public:
    Expression() = default;
    ~Expression() = default;
    virtual float evaluate(Environment* env){return 0;};
    virtual ExpressionType Type(){return ExpressionType::Base;};
};

class Constant : public Expression {
public:
    Constant() = default;
    Constant(float x);
    float evaluate(Environment* env) override;
    ExpressionType Type() override {return ExpressionType::Constant;};
private:
    float v_;
};

using oper = std::function<float(float, float)>;

class Operator {
public:
    Operator() = default;
    Operator(oper, std::string id);
    float apply(float, float);
    std::string id();
private:
    oper op_;
    std::string id_;
};

const std::map<std::string, Operator> predef_operators({
    {"+", Operator([](float x, float y){return x + y;}, "+")},
    {"-", Operator([](float x, float y){return x - y;}, "-")},
    {"*", Operator([](float x, float y){return x * y;}, "*")},
    {"/", Operator([](float x, float y){return x / y;}, "/")},
    {"^", Operator([](float x, float y){return pow(x, y);}, "^")},
});

class Operation : public Expression {
public:
    Operation() = default;
    Operation(std::shared_ptr<Expression> lhs, std::shared_ptr<Expression> rhs, Operator op);
    void set_left(std::shared_ptr<Expression> lhs);
    void set_right(std::shared_ptr<Expression> rhs);
    std::shared_ptr<Expression> get_left();
    std::shared_ptr<Expression> get_right();
    Operator get_op();
    bool is_complete();
    void set_op(Operator op);
    float evaluate(Environment* env) override;
    ExpressionType Type() override {return ExpressionType::Operation;};
private:
    std::shared_ptr<Expression> lhs_ = nullptr;
    std::shared_ptr<Expression> rhs_ = nullptr;
    Operator op_;
};

class Variable : public Expression {
public:
    Variable() = default;
    Variable(std::string name);
    float evaluate(Environment* env) override;
    std::string get_name();
    ExpressionType Type() override {return ExpressionType::Variable;};
private:
    std::string name_;
};

class Environment {
public:
    Environment();
    std::shared_ptr<Expression> get(std::string);
    void add(std::string, std::shared_ptr<Expression>);
private:
    std::map<std::string, std::shared_ptr<Expression>> vars;
};

#endif //RUNTIME_H