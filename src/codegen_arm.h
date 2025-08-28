#pragma once
#include <string>
#include "ast.h"

// Emit minimal ARM Thumb for: int main(){ return <IntLiteral>; }
std::string emitArm(const Program& prog);