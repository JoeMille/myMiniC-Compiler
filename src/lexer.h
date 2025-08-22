#pragma once
#include "tokens.h"
#include <string_view>
#include <vector>
#include <string>

struct LexResult {
    std::vector<Token> tokens;
    std::vector<std::string> errors;
};

LexResult lex(std::string_view source);
