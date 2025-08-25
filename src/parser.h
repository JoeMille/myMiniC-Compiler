#pragma once 
#include "tokens.h"
#include "ast.h"
#include <vector>
#include <string>

struct ParseResult {
    Program program;
    std::vector<std::string> errors;

    bool success() const { return errors.empty(); }
};

ParseResult parse(const std::vector<Token>& tokens);