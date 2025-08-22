#include <iostream>
#include <sstream>
#include "lexer.h"

int main() {
    std::string code = "int main(){ return 42; }";
    LexResult r = lex(code);
    for (auto& t : r.tokens) {
        std::cout << t.line << ":" << t.col << " " << t.lexeme << "\n";
    }
    return 0;
}