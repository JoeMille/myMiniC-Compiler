# myMiniC-Compiler

A deliberately tiny C → ARM Cortex‑M (Thumb) compiler written in modern C++.  
Goal: deepen understanding of (1) ARM Cortex‑M architecture & calling conventions, (2) C & C++ (memory, control flow translation), (3) bare‑metal toolchains & debugging workflow for microcontrollers (e.g. Nordic nRF boards).

---

## 1. Goal

Build a minimal pipeline:
Source (restricted “Mini C”) → Lexer → Parser (AST) → Simple semantic checks → Direct ARM Thumb assembly → Assemble & link with existing GNU tools → Run under emulator (QEMU / Renode) → Eventually flash to real nRF dev kit.

Keep scope frozen; expand only after each stage is stable and understood.

---

## 2. Why This Project Accelerates the Stated Goals

| Goal | How the compiler work teaches it |
|------|----------------------------------|
| ARM Cortex‑M understanding | You must map high‑level constructs to registers (r0–r3 args, r0 return), stack frames, prologue/epilogue, branch & compare instructions, condition flags. Watching your own emitted instructions clarifies what vendor examples obscure. |
| C & C++ mastery | Implementing parsing & AST forces clarity on C grammar; codegen shows how expressions, scopes, and lifetimes translate to runtime stack usage. Using RAII (`std::unique_ptr`) and possibly arenas teaches safe manual memory patterns. |
| Memory management concepts | You will explicitly lay out stack frame offsets for locals, observe alignment, and handle temporary values (spills). Later optimizations (reusing temporaries) reinforce lifetime and ownership reasoning. |
| Microcontroller usage | Toolchain setup, assembling, linking, inspecting ELF, and (later) flashing with OpenOCD / nrfjprog mirrors real firmware workflows. |
| Assembly literacy | You iteratively read & refine the assembly you emit; diff against `arm-none-eabi-gcc -S` for the same snippet to understand alternative instruction selections. |
| Debugging skill | Stepping through your generated code in GDB exposes register state changes tied to language constructs. |

---

## 3. Mini C Subset (Initial Frozen Scope)

- Types: only `int` (32‑bit).
- Storage: local variables (no globals at first).
- Features:
  - Variable declarations with optional initializer.
  - Integer literals.
  - Expressions: + - * / % unary - (later: comparisons, == !=, < > <= >=, logical && ||).
  - Statements: expression `;`, assignment, `return expr;`, `if / else`, `while`, compound `{}`.
  - Functions: start with one (`int main()`); later multiple + calls.
- Exclusions (defer): pointers, arrays, structs, floats, function prototypes, recursion, optimization.

---

## 4. Compiler Stages (Conceptual)

1. Lexing: Convert characters → tokens (identifier, keyword, number, punct).
2. Parsing: Recursive descent builds AST nodes (Function, Block, If, While, Return, BinOp, VarDecl, VarRef, IntLiteral, Assign).
3. Semantic checks: Scope (symbol table stack), uniqueness, simple type check (all int).
4. Code Generation (direct): Walk AST → emit Thumb assembly text.
5. Assemble & link: Use GNU binutils (no custom linker initially; minimal linker script later).
6. Execution / Debug: QEMU / Renode / (later) real board via SWD.

---

## 5. Assembly Mapping (Intuition Bridge)

| C Construct | Assembly Concepts |
|------------|-------------------|
| Local variable | Stack slot: subtract from sp in prologue; access via `ldr/str [sp, #offset]`. |
| Expression `(a+b)*c` | Sequence of loads into r0/r1, arithmetic ops (`add`, `mul`), spills (push/pop) if registers limited. |
| if (cond) {A} else {B} | `cmp`, conditional branch to else label, fallthrough, unconditional branch to end. |
| while (cond) body | Loop label, evaluate cond, branch out, body, branch back. |
| return x; | Place value in r0, restore stack, `bx lr`. |

Seeing these mappings repeatedly builds intuition for performance, stack usage, and side effects.

---

## 6. Learning Milestones (Progression)

1. MVP: Hardcoded `int main(){return 42;}` → valid ELF → run under emulator.
2. Add variables + arithmetic → observe stack growth & register use.
3. Add control flow (if / while) → understand labels & branches / flags.
4. Add comparisons → learn condition flags (NZCV).
5. Add multi-function + calls → implement AAPCS parameter passing & call sequence (bl, stack alignment).
6. Add logical short-circuit → practice label-based evaluation.
7. Optional IR + simple optimizations → appreciate compiler internals & data flow.
8. Flash to real nRF board → integrate startup code, linker script, vector table (bare‑metal context).

---

## 7. Toolchain & Environment (Arch Linux)

Install core packages (Arch):
```
sudo pacman -S --needed \
  arm-none-eabi-gcc arm-none-eabi-binutils arm-none-eabi-newlib \
  qemu-system-arm gdb-multiarch cmake ninja openocd git base-devel
```

Optional / Recommended:
- Renode (more accurate MCU modeling): via AUR (`yay -S renode-bin`) or download from GitHub.
- pyOCD (CMSIS-DAP debug): `pip install pyocd`.
- Nordic tools (for nRF flashing & RTT logging): download & install nRF Command Line Tools (provides `nrfjprog`).
- VS Code extensions: C/C++, CMake Tools, Cortex-Debug.

Verification:
```
arm-none-eabi-gcc --version
qemu-system-arm --version
openocd --version
```

---

## 8. Emulator / Board Workflow (Later Stage)

1. Build compiler → produce `program.s`.
2. Assemble/link:
   ```
   arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb program.s -o program.elf
   ```
3. Emulate (QEMU simple):
   ```
   qemu-system-arm -M stm32-p103 -nographic -kernel program.elf
   ```
   (Later choose machine closer to nRF or use Renode with proper platform script.)
4. Debug:
   ```
   gdb-multiarch program.elf
   (gdb) target remote :1234
   ```
5. Real hardware (later):
   - OpenOCD or `nrfjprog --program program.hex --verify --reset`
   - Use a minimal startup (vector table + Reset_Handler calling main).

---

## 9. Project Structure (Planned)

```
/src
  lexer.*
  tokens.h
  parser.*
  ast.h
  sema.*
  codegen_arm.*
  main.cpp
/tests
  (unit + golden tests)
CMakeLists.txt
README.md
```

---

## 10. Incremental Implementation Order

1. Token enum + lexer with tests.
2. AST & parser for expressions + return.
3. Emit assembly for `return <int>;`.
4. Add variables & assignments.
5. Add arithmetic ops.
6. Add if / while.
7. Add comparisons.
8. Add multiple functions + call/return sequence.
9. Add basic logical ops, then (optional) IR & simple optimizations.

---

## 11. Testing Approach 

- Unit: token sequences, parse trees (shape), codegen snippets.
- Golden end-to-end: Mini C → assembly → ELF; run under QEMU; assert exit code (return value).
- Compare your assembly to `arm-none-eabi-gcc -S` for insight (not for duplication).

---

## 12. Memory Management

- AST lifetime: start with `std::unique_ptr` (RAII).
- Later: implement a monotonic arena (shows trade-offs vs general allocation).
- Stack frame layout: manual offset assignment fosters understanding of alignment & lifetime.
- Temporaries: realize why register allocation matters (pressure = spills).

---

## 13. Guiding Principles

- Keep increments tiny & testable.
- Prefer clarity over cleverness.
- Don’t optimize before correctness.
- Document each new feature in commit messages.

---

## 14. Example Minimal Output (Target)

Input:
```
int main(){ return 42; }
```

Very first acceptable assembly (simplified):
```
  .syntax unified
  .thumb
  .global main
main:
  push {lr}
  movs r0, #42
  pop {pc}
```
