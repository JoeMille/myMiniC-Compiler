#include "parser.h"
#include <cstdlib>   

class Parser {
    const std::vector<Token>& toks;
    size_t pos = 0;
    std::vector<std::string> errors;

 
    const Token& peek(size_t off = 0) const {
        size_t idx = pos + off;
        if (idx >= toks.size()) return toks.back(); // safe End fallback
        return toks[idx];
    }
    bool is(Token::Kind k) const { return peek().kind == k; }

    // Consume expected token or record error
    const Token& consume(Token::Kind k, const char* msg) {
        if (is(k)) return toks[pos++];
        errorAt(peek(), msg);
        return peek(); 
    }

    void errorAt(const Token& t, const std::string& m) {
        errors.push_back("parse error at " +
                         std::to_string(t.line) + ":" +
                         std::to_string(t.col) + " " + m);
    }

    // Expression: only integer literal for now
    std::unique_ptr<Expr> parseExpr() {             
        if (is(Token::Kind::IntLiteral)) {
            const Token& t = consume(Token::Kind::IntLiteral, "expected integer literal");
            auto lit = std::make_unique<IntLiteral>();
            lit->value = std::atoi(t.lexeme.c_str());
            return lit;
        }
        errorAt(peek(), "expected expression");
        auto dummy = std::make_unique<IntLiteral>();
        dummy->value = 0;
        return dummy;                                
    }

    // Return statement: 'return' expr ';'
    std::unique_ptr<ReturnStmt> parseReturn() {      
        consume(Token::Kind::KwReturn, "expected 'return'");
        auto r = std::make_unique<ReturnStmt>();
        r->expr = parseExpr();
        consume(Token::Kind::Semicolon, "expected ';' after return expression");
        return r;                                   
    }

    // Function: int <id>() { return <int>; }
    Function parseFunction() {
        consume(Token::Kind::KwInt, "expected 'int' at function start");
        const Token& nameTok = consume(Token::Kind::Identifier, "expected function name");
        consume(Token::Kind::LParen, "expected '('");
        consume(Token::Kind::RParen, "expected ')'");
        consume(Token::Kind::LBrace, "expected '{'");
        auto ret = parseReturn();                     
        consume(Token::Kind::RBrace, "expected '}' after function body");

        Function f;
        f.name = nameTok.lexeme;
        f.body = std::move(ret);                    
        return f;                                     
    }

public:
    Parser(const std::vector<Token>& t) : toks(t) {}

    ParseResult run() {
        ParseResult pr;
        if (!is(Token::Kind::End)) {
            pr.program.functions.push_back(parseFunction());
        }
        if (!is(Token::Kind::End)) {
            errorAt(peek(), "unexpected tokens after function");
        }
        pr.errors = std::move(errors);
        return pr;
    }
};

ParseResult parse(const std::vector<Token>& tokens) {
    Parser p(tokens);
    return p.run();
}