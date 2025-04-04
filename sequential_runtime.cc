#include "runtime.h"
#include "log.h"

// class Constant

Constant::Constant(float x){
    v_ = x;
}

float Constant::evaluate(Environment* env) {
    env->increment_evaled_constants(/*thread_id=*/0);
    return v_;
}

float Constant::get() {
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
    LOG(1, "Evaluating operation " <<  op_.id() << " with " << lhs_eval << ", " << rhs_eval << "\n")
    env -> increment_evaled_operators(/*thread_id=*/0);
    return op_.apply(lhs_eval, rhs_eval);
}

Variable::Variable(std::string name) {
    name_ = name;
}

float Variable::evaluate(Environment* env) {
    env -> increment_evaled_variables(/*thread_id=*/0);
    if (env -> get(name_) == nullptr) {
        throw std::string("Variable " + name_ + " is not set.");
    }
    return env -> get(name_) -> evaluate(env);
}

std::string Variable::get_name() {
    return name_;
}

Environment::Environment() {
    vars.insert({"pi", std::make_shared<Constant>(M_PI)});
    vars.insert({"e", std::make_shared<Constant>(M_E)});
}

std::shared_ptr<Expression> Environment::get(std::string s) {
    if(vars.find(s) != vars.end()) {
        return vars.at(s);
    }

    LOG(1, "No identifier " << s << " in environment.\n")
    return nullptr;
}

void Environment::add(std::string s, std::shared_ptr<Expression> exp){
    LOG(3, "inserting: " << s << ": " << exp << std::endl)
    vars[s] = exp;
    // dump();
}