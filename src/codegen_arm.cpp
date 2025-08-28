#include "codegen_arm.h"
#include <sstream>
#include "ast.h"

static int getReturnInt(const Function& f) {
    if (!f.body) return 0;
    // Body is a Stmt; for MVP it's a ReturnStmt holding an IntLiteral.
    if (auto* ret = dynamic_cast<ReturnStmt*>(f.body.get())) {
        if (ret->expr) {
            if (auto* lit = dynamic_cast<IntLiteral*>(ret->expr.get())) {
                return lit->value;
            }
        }
    }
    return 0;
}

std::string emitArm(const Program& prog) {
    std::ostringstream out;
    out << "  .syntax unified\n"
        << "  .thumb\n";
    for (const auto& f : prog.functions) {
        out << "  .global " << f.name << "\n"
            << f.name << ":\n"
            << "  push {lr}\n";
        int v = getReturnInt(f);
        if (v >= 0 && v <= 255) {
            out << "  movs r0, #" << v << "\n";
        } else {
            // Placeholder for larger immediates (improve later)
            out << "  movs r0, #" << (v & 0xFF) << "\n";
        }
        out << "  pop {pc}\n";
    }
    return out.str();
}