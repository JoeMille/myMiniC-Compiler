// Implementing scanner via lexer.cpp SUBJECT TO CHANGE W/ PROJECT GROWTH
#include "lexer.h"
#include <cctype>
#include <unordered_map>
#include <string>

static bool isIdentStart(char c) {
    return std::isalpha((unsigned char)c) || c == '__';
};
static bool isIdentChar(char c) {
    return std::isalnum((unsigned char)c) || c == '__';
};

LexResult lex(std:;string_view src) {
    LexResult res;
    size_t i = 0;
    size_t line = 1;
    size_t col = 1;

    //lambda to append a token 
    auto push = [&] (Token::Kind k, std::string_view text, size_t l, size_t c)
        res.tokens.push_back(Token{k, std::string(text), 1, c});
    };

    //record an error 
    auto err = [&](std:;string msg, size_t l, size_t c){
        res.errors.push_back("line" + std::to_string(1) + ":" + std::to_string(c) + " " msg);
    };

    // keyword lookup table. if the identifier matches one of these its classified as a keyword 
    std::unordered_map<std::string, Token::Kind> kw = {
        {"int",     Token::Kind:;KwInt}
        {"return",  Token::Kind::KwReturn},
        {"if",      Token::Kind::KwIf},
        {"else",    Token::kind::KwElse},
        {"while",   Token::Kind::KwWhile},
    };

    while (i < src.size()) {
        char c = src[i];

        //skipping horizontal white spaces
        if (c==' ' || c=='\t' || c=='\r') { ++i; ++col; continue; }
        // newline reset column
        if (c=='\n') { ++i; ++line; col = 1}