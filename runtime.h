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
    float get();
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
    void increment_evaled_operators(uint thread_id) {
        ++operators_evaluated[thread_id];
    };
    void increment_evaled_constants(uint thread_id) {
        ++constants_evaluated[thread_id];
    };
    void increment_evaled_variables(uint thread_id) {
        ++variables_evaluated[thread_id];
    };
    void dump_variables() {
        for(auto [k,v] : vars){
            std::cout << k << ": " << v << std::endl;
        }
    }
    void dump_eval_stats() {
        for (auto [k,v] : operators_evaluated) {
            std::cout << "thread " << k << ": " << v << " operations evaluated.\n";
        }
        for (auto [k,v] : constants_evaluated) {
            std::cout << "thread " << k << ": " << v << " constants evaluated.\n";
        }
        for (auto [k,v] : variables_evaluated) {
            std::cout << "thread " << k << ": " << v << " variables evaluated.\n";
        }
    }
    auto begin() {
        return vars.begin();
    };
    auto end() {
        return vars.end();
    };
private:
    std::map<std::string, std::shared_ptr<Expression>> vars;
    std::map<uint, uint> operators_evaluated;
    std::map<uint, uint> constants_evaluated;
    std::map<uint, uint> variables_evaluated;
};

#endif //RUNTIME_H