#include <iostream>
#include <string>
#include <algorithm>
#include <map>
#include <vector>
#include <set>
#include <stack>
#include <cmath>
#include <cstdlib>

#include "expression.h"

void remove_char(std::string& x, char c) {
    x.erase(std::remove(x.begin(), x.end(), ' '), x.end());
}

std::vector<std::string> tokenize(std::string expr) {
    std::vector<std::string> tokens;
    std::string cur_token = "";
    for (char c : expr) {
        if (isdigit(c)) {
            cur_token += c;
        } else {
            tokens.push_back(cur_token);
            tokens.push_back(std::string(1, c));
            cur_token = "";
        }
    }
    tokens.push_back(cur_token);
    return tokens;
}
 
std::shared_ptr<Expression> tokens_to_expr(std::vector<std::string> tokens) { 
    std::shared_ptr<Operation> last_oper;
    std::stack<Expression*> exp_stack;
    for (auto tokenp = tokens.begin(); tokenp != tokens.end(); ++tokenp) {
        auto token = *tokenp;
        Expression *uninserted = nullptr;
        if(token == "("){
            auto token_iter = tokenp + 1;
            int num_open_paren = 1;
            while(num_open_paren > 0 && token_iter != tokens.end()){
                if (*token_iter == ")") {
                    --num_open_paren;
                } else if (*token_iter == "(") {
                    ++num_open_paren;
                }
                ++token_iter;
            }
            if(num_open_paren > 0){
                std::cout << "Error: unmatched parens\n";
                return nullptr;
            }
            std::shared_ptr<Expression> in_paren =
                tokens_to_expr(std::vector<std::string>(tokenp + 1, token_iter - 1));
            auto zero = std::make_shared<Constant>(0);
            Operation* paren_oper = new Operation(zero, in_paren, predef_operators.at("+"));
            uninserted = paren_oper;
            tokenp = token_iter;
        } else if(isdigit(token[0])) {
            uninserted = new Constant(std::stof(token));
        } else {
            if(is_high_prio_operator(token)) {
                auto top = exp_stack.top();
                std::cout << "is high " << std::endl;
                std::cout << "top is a: " << (int)top->Type() << std::endl;
                if(Expr::is_constant(top)){
                    Operation *o = new Operation();
                    o -> set_op(predef_operators.at(token));
                    o -> set_left(std::make_shared<Constant>(top->evaluate()));
                    exp_stack.pop();
                    exp_stack.push(o);
                    delete top;
                } else if(Expr::is_operation(top)) {
                    auto opr = (Operation*)top;
                    if(!opr -> is_complete()){
                        std::cout << "Error: can't honor PEMDAS with an incomplete previous operation.\n";
                        return nullptr;
                    }
                    Operation *o = new Operation();
                    o -> set_op(predef_operators.at(token));
                    o -> set_left(opr->get_right());
                    opr->set_right(nullptr);
                    exp_stack.push(o);
                }
            } else if(is_low_prio_operation(token)) {
                auto top = exp_stack.top();
                exp_stack.pop();
                std::cout << "is low " << std::endl;
                std::cout << "top is a: " << (int)top->Type() << std::endl;
                Operation *o = new Operation();
                if(Expr::is_constant(top)){
                    o -> set_left(std::make_shared<Constant>(top -> evaluate()));
                    delete top;
                } else if (Expr::is_operation(top)) {
                    Operation* toper = (Operation*)top;
                    o -> set_left(std::make_shared<Operation>(toper -> get_left(), toper -> get_right(), toper -> get_op()));
                    delete toper;
                }
                o -> set_op(predef_operators.at(token));
                exp_stack.push(o);
            }
        }
        
        if (uninserted != nullptr) {
            if (exp_stack.empty()) {
                exp_stack.push(uninserted);
            } else {
                auto top = exp_stack.top();
                std::cout << "type_id: " << typeid(*top).name() << "\n";
                if(Expr::is_constant(top)) {
                    std::cout << "Error: constant/paren came after constant\n";
                    return nullptr;
                } else if(Expr::is_operation(top)) {
                    auto opr = (Operation*)top;
                    if (opr -> is_complete()){
                        std::cout << "Error: can't add constant/paren to complete operation.\n";
                        return nullptr;
                    }
                    if(Expr::is_constant(uninserted)){
                        opr -> set_right(std::make_shared<Constant>(uninserted -> evaluate()));
                    } else if (Expr::is_operation(uninserted)){
                        Operation* paren_op = (Operation*) uninserted;
                        opr -> set_right(
                            std::make_shared<Operation>(paren_op -> get_left(), paren_op -> get_right(), paren_op -> get_op()));
                    }
                    std::string opr_id = opr -> get_op().id();
                    if(is_high_prio_operator(opr_id)){
                        if(exp_stack.size() > 1){
                            exp_stack.pop();
                            if(Expr::is_operation(exp_stack.top())){
                                Operation* top_2 = (Operation*)exp_stack.top();
                                std::string top_2_opr = top_2 -> get_op().id();
                                if(is_high_prio_operator(opr_id)){
                                    top_2 -> set_right(std::make_shared<Operation>(
                                        opr -> get_left(),
                                        opr -> get_right(),
                                        opr -> get_op()
                                    ));
                                    delete opr;
                                } else {
                                    exp_stack.push(opr);
                                }
                            } else {
                                exp_stack.push(opr);
                            }
                        }
                    }
                }
            }
        }
        std::cout << "stack size: " << exp_stack.size() << std::endl;
        // std::cout << "value of top is " << exp_stack.top()->evaluate() << std::endl;
    }

    while(exp_stack.size() > 1){
        auto top = exp_stack.top();
        exp_stack.pop();
        if(Expr::is_operation(top)) {
            Operation* toper = (Operation*)top;
            ((Operation*)exp_stack.top()) -> set_right(
                std::make_shared<Operation>(
                    toper -> get_left(), toper -> get_right(), toper -> get_op()));
        } else if(Expr::is_constant(top)) {
            ((Operation*)exp_stack.top()) -> set_right(
                std::make_shared<Constant>(top -> evaluate()));
        }
        delete top;
    }

    std::cout << "returning with stack size: " << exp_stack.size() << std::endl;

    auto top = exp_stack.top();
    if (Expr::is_constant(top)) {
        std::cout << "top is a const\n";
        return std::make_shared<Constant>(top -> evaluate());
    } else {
        Operation* toper = (Operation*)top;
        return std::make_shared<Operation>(toper -> get_left(), toper -> get_right(), toper -> get_op());
    }
    
}

int main() {
    std::string x = "( 1 + 2 ) * ( ( ( 1 + 2 ) ^ 3 ) + 4 )";
    // std::string x = "1 + 2 + 3";
    // std::string x = "1234 * 1234";
    // std::string x = "123 123";
    remove_char(x, ' ');
    std::cout << x << "\n";

    auto tokens = tokenize(x);
    for (auto token : tokens) {
        std::cout << token << " ";
    }

    std::cout << "\n";

    auto e = tokens_to_expr(tokens);

    std::cout << "expr = " << e -> evaluate() << "\n";

    std::cout << std::endl;
    return 0;
}