#ifndef UTILS_H
#define UTILS_H

#include <set>
#include <string>

#include "runtime.h"

namespace Expr {
    bool is_constant(Expression* e);
    bool is_operation(Expression* e);
    bool is_high_prio_operator(std::string token);
    bool is_low_prio_operation(std::string token);
}

#endif