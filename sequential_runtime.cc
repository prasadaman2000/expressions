#include "runtime.h"

// class Constant

Constant::Constant(float x){
    v_ = x;
}

float Constant::evaluate(Environment* _) {
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

float Operation::evaluate(Environment* env) {
    // std::cout << "Evaluating operation " <<  op_.id() << "with " << lhs_ << " " << rhs_ << "\n";
    float lhs_eval = lhs_ -> evaluate(env);
    float rhs_eval = rhs_ -> evaluate(env);
    std::cout << "Evaluating operation " <<  op_.id() << " with " << lhs_eval << ", " << rhs_eval << "\n";
    return op_.apply(lhs_eval, rhs_eval);
}

Variable::Variable(std::string name) {
    name_ = name;
}

float Variable::evaluate(Environment* env) {
    return env -> get(name_) -> evaluate(env);
}

std::string Variable::get_name() {
    return name_;
}

Environment::Environment() {
    vars.insert({"pi", std::make_shared<Constant>(3.14159265)});
}

std::shared_ptr<Expression> Environment::get(std::string s) {
    if(vars.find(s) != vars.end()) {
        return vars.at(s);
    }

    std::cout << "No identifier " << s << " in environment.\n";
    return nullptr;
}

void Environment::add(std::string s, std::shared_ptr<Expression> exp){
    vars.insert({s, exp});
}