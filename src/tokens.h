#pragma once
#include <string>

struct Token {
    enum class Kind {
        End,
        Indentifier,
        IntLiteral,

        // keywords
        KwInt, KwReturn, KwIf, KwElse, KwWhile,

        // single char punct operators
        LParen, RParen,
        LBrace, RBrace,
        Semicolon, Comma, 
        Plus, Minus, Star, Slash, Percent,
        Assign
    };

    Kind kind;
    std::string lexeme;
    size_t line;
    size_t col;
};