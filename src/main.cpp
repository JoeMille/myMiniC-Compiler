#include <iostream>
#include <sstream>
#include "lexer.h"
#include "parser.h"
#include "codegen_arm.h"

int main() {
    std::string code = "int main(){ return 42; }";
    LexResult r = lex(code);
    for (auto& t : r.tokens) {
        std::cout << t.line << ":" << t.col << " " << t.lexeme << "\n";
    }

    ParseResult pr = parse(r.tokens);
    if(!pr.errors.empty()) {
        std::cerr << "Parse errors:\n";
        for (auto& e : pr.errors) std::cerr << " " << e << "\n";
        return 1;   
    }
    if (!pr.program.functions.empty()) {
        std::cout << "Parsed function: " << pr.program.functions[0].name << "\n";
        std::cout << "\n--- ARM Thumb ---\n" << emitArm(pr.program) << "\n";
    }
    return 0;
}