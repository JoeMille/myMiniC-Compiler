#pragma once
#include <string>
#include <vector>

struct Token {
    enum class Kind {
        End,
        Identifier,       
        IntLiteral,
        KwInt, KwReturn, KwIf, KwElse, KwWhile,
        LParen, RParen, LBrace, RBrace, Semicolon, Comma,
        Plus, Minus, Star, Slash, Percent,
        Assign
    };

    Kind kind;
    std::string lexeme;
    size_t line;
    size_t col;
};