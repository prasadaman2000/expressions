#include "runtime.h"

// class Constant

Constant::Constant(float x){
    v_ = x;
}

float Constant::evaluate() {
    return v_;
}

//class Operator

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

// class Operation

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