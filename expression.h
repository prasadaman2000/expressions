#include <functional>
#include <memory>
#include <map>
#include <iostream>
#include <cmath>

enum class ExpressionType {Base, Constant, Operation};

class Expression {
public:
    Expression() = default;
    ~Expression() = default;
    virtual float evaluate(){return 0;};
    virtual ExpressionType Type(){return ExpressionType::Base;};
};

class Constant : public Expression {
public:
    Constant() = default;
    Constant(float x);
    float evaluate() override;
    ExpressionType Type() override {return ExpressionType::Constant;};
private:
    float v_;
};

Constant::Constant(float x){
    v_ = x;
}

float Constant::evaluate() {
    return v_;
}

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

Operator::Operator(oper f, std::string id) {
    op_ = f;
    id_ = id;
}

float Operator::apply(float x, float y){
    return op_(x,  y);
}

std::string Operator::id() {
    return id_;
}

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
    float evaluate() override;
    ExpressionType Type() override {return ExpressionType::Operation;};
private:
    std::shared_ptr<Expression> lhs_ = nullptr;
    std::shared_ptr<Expression> rhs_ = nullptr;
    Operator op_;
};

Operation::Operation(std::shared_ptr<Expression> lhs, std::shared_ptr<Expression> rhs, Operator op) {
    lhs_ = lhs;
    rhs_ = rhs;
    op_ = op;
}

void Operation::set_right(std::shared_ptr<Expression> rhs) {
    rhs_ = rhs;
}

void Operation::set_left(std::shared_ptr<Expression> lhs) {
    lhs_ = lhs;
}

void Operation::set_op(Operator op){
    op_ = op;
}

std::shared_ptr<Expression> Operation::get_left() {
    return lhs_;
}

std::shared_ptr<Expression> Operation::get_right() {
    return rhs_;
}

Operator Operation::get_op() {
    return op_;
}

bool Operation::is_complete() {
    return (lhs_ != nullptr) && (rhs_ != nullptr);
}

float Operation::evaluate() {
    // std::cout << "Evaluating operation " <<  op_.id() << "with " << lhs_ << " " << rhs_ << "\n";
    float lhs_eval = lhs_ -> evaluate();
    float rhs_eval = rhs_ -> evaluate();
    std::cout << "Evaluating operation " <<  op_.id() << " with " << lhs_eval << ", " << rhs_eval << "\n";
    return op_.apply(lhs_eval, rhs_eval);
}

namespace Expr {
    bool is_constant(Expression* e) {
        std::cout << "is constant: " << (e -> Type() == ExpressionType::Constant) << std::endl;
        return (e -> Type()) == ExpressionType::Constant;
    }

    bool is_operation(Expression* e) {
        std::cout << "is operation: " << (e -> Type() == ExpressionType::Operation) << std::endl;
        return (e -> Type()) == ExpressionType::Operation;
    }
}

std::set<std::string> high_priority_operators({"^", "*", "/"});
std::set<std::string> low_priority_operators({"+", "-"});

bool is_high_prio_operator(std::string token) {
    return high_priority_operators.find(token) != high_priority_operators.end();
}

bool is_low_prio_operation(std::string token) {
    return low_priority_operators.find(token) != low_priority_operators.end();
}