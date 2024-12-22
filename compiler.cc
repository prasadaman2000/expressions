#include "compiler.h"
#include "log.h"
#include "utils.h"

#include <algorithm>
#include <map>
#include <vector>
#include <set>
#include <stack>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <filesystem>

void remove_char(std::string& x, char c) {
    x.erase(std::remove(x.begin(), x.end(), ' '), x.end());
}

std::vector<std::string> tokenize(std::string expr) {
    std::vector<std::string> tokens;
    std::string cur_token = "";
    for (char c : expr) {
        if (isalnum(c)) {
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

bool is_number(std::string s){
    for (char c : s){
        if(!isdigit(c)){
            return false;
        }
    }
    return s.size() > 0;
}

bool is_word(std::string s) {
    for (char c : s){
        if(!isalnum(c)){
            return false;
        }
    }
    return true;
}

bool is_whitespace(std::string s) {
    for(auto c : s) {
        if(!std::isspace(c)) {
            return false;
        }
    }
    return true;
}

std::vector<std::string> read_func_expr_from_file(std::string func_name) {
    std::filesystem::path file_path = "./functions/" + func_name + ".func";
    if(!std::filesystem::exists(file_path)) return std::vector<std::string>();
    std::ifstream file(file_path);
    std::stringstream buffer;
    buffer << file.rdbuf();

    std::string program_str = buffer.str();
    remove_char(program_str, ' ');
    return tokenize(program_str);
}

std::shared_ptr<Expression> tokens_to_expr(std::vector<std::string> tokens, Environment* env, std::string scope) { 
    LOG(3, "entering tokens_to_expr\n")
    std::shared_ptr<Operation> last_oper;
    std::stack<Expression*> exp_stack;
    for (auto tokenp = tokens.begin(); tokenp != tokens.end(); ++tokenp) {
        auto token = *tokenp;
        Expression *uninserted = nullptr;
        LOG(3, "Processing token " << token << std::endl)
        if(is_whitespace(token)) {
            LOG(4, "Skipping whitespace token [" << token << "]\n")
            continue;
        }
        // if parentheses, create expression out of everything in between the parens
        // creates a  (0 + expr) in order to support the RHS stealing that comes
        // out of multiplications
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
            LOG(4, "entering paren expression with stack size " << exp_stack.size() << std::endl);
            std::shared_ptr<Expression> in_paren =
                tokens_to_expr(std::vector<std::string>(tokenp + 1, token_iter - 1), env, scope);
            LOG(4, "exiting paren expression with stack size " << exp_stack.size() << std::endl)
            auto zero = std::make_shared<Constant>(0);
            Operation* paren_oper = new Operation(zero, in_paren, predef_operators.at("+"));
            uninserted = paren_oper;
            tokenp = token_iter;
        } else if(is_number(token)) {
            LOG(4, "stof on " << token << std::endl)
            uninserted = new Constant(std::stof(token));
        } else if(is_word(token)) {
            auto tokenized_file = read_func_expr_from_file(token);
            if(tokenized_file.size() > 0){
                std::string scope_suffix = std::to_string(rand());
                auto func_expr = tokens_to_expr(tokenized_file, env, scope + scope_suffix);
                auto zero = std::make_shared<Constant>(0);
                Operation* paren_oper = new Operation(zero, func_expr, predef_operators.at("+"));
                std::shared_ptr<Expression> in_paren;
                if(*(tokenp + 1) == "(") {
                    auto token_iter = tokenp + 2;
                    int num_open_paren = 1;
                    while(num_open_paren > 0 && token_iter != tokens.end()){
                        LOG(5, "token_iter = " << *(token_iter) << "\n");
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
                    LOG(4, "generating expr for paren for func from token " << *(tokenp+2) << " to " << *(token_iter - 1) << "\n")
                    in_paren = tokens_to_expr(std::vector<std::string>(tokenp + 2, token_iter - 1), env, scope);
                    LOG(4, "generated expr for paren for func\n");
                    tokenp = token_iter;
                }
                env -> add("x" + scope + scope_suffix, in_paren);
                uninserted = paren_oper;
            } else {
                // if the variable is a predefined constant, keep the original name, else make it a scoped variable
                if (env -> get(token) != nullptr) {
                    uninserted = new Variable(token);
                } else {
                    std::string var_name = token + scope;
                    uninserted = new Variable(token + scope);
                    env -> add(var_name, nullptr);
                }
            }
        } else {
            // if operation is a * / ^, take the RHS of the last operation and make it
            // the current operation's LHS. Push operation onto stack
            if(Expr::is_high_prio_operator(token)) {
                auto top = exp_stack.top();
                LOG(5, "is high " << std::endl);
                LOG(5, "top is a: " << (int)top->Type() << std::endl);
                if(Expr::is_constant(top)){
                    Operation *o = new Operation();
                    o -> set_op(predef_operators.at(token));
                    o -> set_left(std::make_shared<Constant>(top->evaluate(env)));
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
                } else if(Expr::is_variable(top)) {
                    Operation *o = new Operation();
                    o -> set_op(predef_operators.at(token));
                    o -> set_left(std::make_shared<Variable>(((Variable*)(top)) -> get_name()));
                    exp_stack.pop();
                    exp_stack.push(o);
                    delete top;
                }
            } else if(Expr::is_low_prio_operation(token)) {
                // if low priority operation, then naively take
                // last thing on stack, and make it the LHS
                auto top = exp_stack.top();
                exp_stack.pop();
                LOG(5, "is low " << std::endl)
                LOG(5, "top is a: " << (int)top->Type() << std::endl)
                Operation *o = new Operation();
                if(Expr::is_constant(top)){
                    o -> set_left(std::make_shared<Constant>(top -> evaluate(env)));
                    delete top;
                } else if (Expr::is_operation(top)) {
                    Operation* toper = (Operation*)top;
                    o -> set_left(std::make_shared<Operation>(toper -> get_left(), toper -> get_right(), toper -> get_op()));
                    delete toper;
                } else if (Expr::is_variable(top)) {
                    Variable* topvar = (Variable*)top;
                    o -> set_left(std::make_shared<Variable>(topvar -> get_name()));
                }
                o -> set_op(predef_operators.at(token));
                exp_stack.push(o);
            }
        }
        
        // if the last thing was uninserted (paren exp or a constant)
        //  if stack is empty, put it on stack
        //  if the last thing on the stack is an operation
        //      add uninserted as RHS of top of stack
        //          if top of stack is high prio and second from top of stack is high prio
        //              make top of stack the RHS of second from top
        //              this ensures that all consecutive high priority operations
        //              are grouped and executed together
        if (uninserted != nullptr) {
            LOG(4, "uninserted not null AND stack size is: " << exp_stack.size() << std::endl);
            if (exp_stack.empty()) {
                exp_stack.push(uninserted);
            } else {
                auto top = exp_stack.top();
                LOG(4, "top type_id: " << typeid(*top).name() << "\n");
                if(Expr::is_constant(top) || Expr::is_variable(top)) {
                    std::cout << "Error: constant/paren came after constant/variable\n";
                    return nullptr;
                } else if(Expr::is_operation(top)) {
                    auto opr = (Operation*)top;
                    if (opr -> is_complete()){
                        std::cout << "Error: can't add constant/paren to complete operation.\n";
                        return nullptr;
                    }
                    if(Expr::is_constant(uninserted)){
                        opr -> set_right(std::make_shared<Constant>(uninserted -> evaluate(env)));
                    } else if (Expr::is_operation(uninserted)){
                        Operation* paren_op = (Operation*) uninserted;
                        opr -> set_right(
                            std::make_shared<Operation>(paren_op -> get_left(), paren_op -> get_right(), paren_op -> get_op()));
                    } else if (Expr::is_variable(uninserted)){
                        Variable* topvar = (Variable*) uninserted;
                        opr -> set_right(std::make_shared<Variable>(topvar -> get_name()));
                    }
                    std::string opr_id = opr -> get_op().id();
                    if(Expr::is_high_prio_operator(opr_id)){
                        if(exp_stack.size() > 1){
                            exp_stack.pop();
                            if(Expr::is_operation(exp_stack.top())){
                                Operation* top_2 = (Operation*)exp_stack.top();
                                std::string top_2_opr = top_2 -> get_op().id();
                                if(Expr::is_high_prio_operator(opr_id)){
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
        LOG(3, "stack size: " << exp_stack.size() << std::endl);
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
                std::make_shared<Constant>(top -> evaluate(env)));
        } else if(Expr::is_variable(top)) {
            Variable* topvar = (Variable*)top;
            ((Operation*)exp_stack.top()) -> set_right(std::make_shared<Variable>(topvar -> get_name()));
        }
        delete top;
    }

    LOG(5, "returning with stack size: " << exp_stack.size() << std::endl);

    auto top = exp_stack.top();
    if (Expr::is_constant(top)) {
        LOG(3, "returning a const\n");
        return std::make_shared<Constant>(top -> evaluate(env));
    } else if(Expr::is_operation(top)) {
        LOG(3, "returning an operation\n");
        Operation* toper = (Operation*)top;
        return std::make_shared<Operation>(toper -> get_left(), toper -> get_right(), toper -> get_op());
    } else if(Expr::is_variable(top)) {
        LOG(3, "returning a variable\n");
        Variable* topvar = (Variable*)top;
        return std::make_shared<Variable>(topvar -> get_name());
    }
    return nullptr; // should never get here
}

Program Compiler::compile(std::string program, Environment* env) {
    remove_char(program, ' ');
    LOG(5, program << "\n");

    auto tokens = tokenize(program);
    for (auto token : tokens) {
        LOG(5, token << " ");
    }

    LOG(5, "\n");

    return Program(tokens_to_expr(tokens, env, ""));
}

Program::Program(std::shared_ptr<Expression> root){
    root_ = root;
}

float Program::execute(Environment* env) {
    return root_ -> evaluate(env);
}
