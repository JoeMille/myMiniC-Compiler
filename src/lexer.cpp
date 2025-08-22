#include "lexer.h"
#include <cctype>
#include <unordered_map>
#include <string>

// Helpers: what characters can start / continue an identifier
static bool isIdentStart(char c) {
    return std::isalpha(static_cast<unsigned char>(c)) || c == '_';
}
static bool isIdentChar(char c) {
    return std::isalnum(static_cast<unsigned char>(c)) || c == '_';
}

LexResult lex(std::string_view src) {
    LexResult res;
    size_t i = 0;
    size_t line = 1;
    size_t col  = 1;

    // Append a token
    auto push = [&](Token::Kind k, std::string_view text, size_t l, size_t c) {
        res.tokens.push_back(Token{k, std::string(text), l, c});
    };

    // Record an error
    auto err = [&](const std::string& msg, size_t l, size_t c) {
        res.errors.push_back("line " + std::to_string(l) + ":" + std::to_string(c) + " " + msg);
    };

    // Keyword lookup table
    std::unordered_map<std::string, Token::Kind> kw = {
        {"int",    Token::Kind::KwInt},
        {"return", Token::Kind::KwReturn},
        {"if",     Token::Kind::KwIf},
        {"else",   Token::Kind::KwElse},
        {"while",  Token::Kind::KwWhile},
    };

    while (i < src.size()) {
        char c = src[i];

        // Skip horizontal whitespace
        if (c == ' ' || c == '\t' || c == '\r') { ++i; ++col; continue; }
        // Newline
        if (c == '\n') { ++i; ++line; col = 1; continue; }

        size_t startLine = line;
        size_t startCol  = col;

        // Identifier / keyword
        if (isIdentStart(c)) {
            size_t start = i;
            ++i; ++col;
            while (i < src.size() && isIdentChar(src[i])) { ++i; ++col; }
            std::string word(src.substr(start, i - start));
            auto it = kw.find(word);
            push(it != kw.end() ? it->second : Token::Kind::Identifier, word, startLine, startCol);
            continue;
        }

        // Integer literal (decimal only)
        if (std::isdigit(static_cast<unsigned char>(c))) {
            size_t start = i;
            ++i; ++col;
            while (i < src.size() && std::isdigit(static_cast<unsigned char>(src[i]))) { ++i; ++col; }
            push(Token::Kind::IntLiteral, src.substr(start, i - start), startLine, startCol);
            continue;
        }

        // One‑char tokens
        auto single = [&](Token::Kind k) {
            push(k, src.substr(i, 1), startLine, startCol);
            ++i; ++col;
        };

        switch (c) {
            case '(': single(Token::Kind::LParen);    continue;
            case ')': single(Token::Kind::RParen);    continue;
            case '{': single(Token::Kind::LBrace);    continue;
            case '}': single(Token::Kind::RBrace);    continue;
            case ';': single(Token::Kind::Semicolon); continue;
            case ',': single(Token::Kind::Comma);     continue;
            case '+': single(Token::Kind::Plus);      continue;
            case '-': single(Token::Kind::Minus);     continue;
            case '*': single(Token::Kind::Star);      continue;
            case '/': single(Token::Kind::Slash);     continue;
            case '%': single(Token::Kind::Percent);   continue;
            case '=': single(Token::Kind::Assign);    continue;
        }

        // Unrecognized character
        err(std::string("unexpected character '") + c + "'", startLine, startCol);
        ++i; ++col;
    }

    // End token
    push(Token::Kind::End, "", line, col);
    return res;
}