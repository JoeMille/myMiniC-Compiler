// include this header just the once during compilation
#pragma once


#include <memory>
#include <string>
#include <vector>

// base node gives a common ancestor useful for virtual destruction
struct Node { virtual ~Node() = default; };

// any copmutable value 
struct Expr : Node {};

// actions (ie return, if, while etc.,)
struct Stmt : Node {};

struct IntLiteral : Expr {
    int value;
};

// return statements 
struct ReturnStmt : Stmt {
    std::unique_ptr<Expr> expr;
};

struct Function {
    std:: string name;
    std::unique_ptr<Stmt> body;
};

// program route for all top level functions
struct Program {
    std::vector<Function> functions;
};