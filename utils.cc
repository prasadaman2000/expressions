#include <set>
#include <string>
#include "utils.h"
#include "runtime.h"
#include "log.h"

std::set<std::string> high_priority_operators({"^", "*", "/"});
std::set<std::string> low_priority_operators({"+", "-"});

bool Expr::is_high_prio_operator(std::string token) {
    return high_priority_operators.find(token) != high_priority_operators.end();
}

bool Expr::is_low_prio_operation(std::string token) {
    return low_priority_operators.find(token) != low_priority_operators.end();
}

bool Expr::is_constant(Expression* e) {
    LOG(2, "is constant: " << ((e -> Type()) == ExpressionType::Constant) << std::endl)
    return (e -> Type()) == ExpressionType::Constant;
}

bool Expr::is_operation(Expression* e) {
    LOG(2, "is operation: " << (e -> Type() == ExpressionType::Operation) << std::endl)
    return (e -> Type()) == ExpressionType::Operation;
}

bool Expr::is_variable(Expression* e) {
    LOG(2, "is variable: " << ((e -> Type()) == ExpressionType::Variable) << std::endl)
    return (e -> Type()) == ExpressionType::Variable;
}