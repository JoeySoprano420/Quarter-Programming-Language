# Quarter-Programming-Language

# 🌟 Quarter Programming Language (v2.1)

> *Readable like English. Powerful like Assembly. Beautiful like poetry.*

---

## 🚀 Overview
**QuarterLang** is a new programming language built for maximum readability, structural consistency, and direct executability. 
It introduces a blend of functional, imperative, and itemized paradigms using fluent, compact English-like syntax.

Supports AOT compilation, split/toggled JIT execution, and inline NASM. Also introduces:
- **DGs** (Dodecagrams): base-12 numeric system
- **Derivatives**: transformation-based variable definitions
- **Capsules**: compressed execution bundles

---

## 🧠 Language Design Paradigms
- **ITO** – Itemized Transformative Objective
- **FD** – Functional Directive
- **SII** – Sequential Iterative Imperative
- **LP** – Lateral Procedural
- **LSFS** – Longitudinal Scope Full Stack

---

## 📘 Program Structure
```quarter
star
  ...your code here...
end
```

**All programs start with `star` and end with `end`. Every open block must be closed.**

---

## 🔤 Core Keywords
| Keyword     | Purpose                                   |
|-------------|-------------------------------------------|
| `star`      | Begin program block                       |
| `end`       | End program block                         |
| `val`       | Immutable variable                        |
| `var`       | Mutable variable                          |
| `say`       | Print to output                           |
| `loop`      | For loop                                  |
| `when`      | If conditional                            |
| `else`      | Else branch                               |
| `derive`    | Derivative assignment                     |
| `dg`        | Dodecagram base-12 numeric type           |
| `match`     | Pattern matching                          |
| `case`      | Match case                                |
| `define`    | Function definition                       |
| `procedure` | Alternative to define                     |
| `return`    | Return from function                      |
| `yield`     | Yield value                               |
| `thread`    | Spawn parallel thread                     |
| `pipe`      | Pipe to stream or file                    |
| `nest`      | Inline block encapsulation                |
| `fn`        | Anonymous function                        |
| `asm`       | Inline assembly                           |
| `stop`      | Immediate halt                            |

---

## 🔢 Data Types
- `int` — whole numbers
- `float` — decimals with precision
- `string` — text
- `bool` — true/false
- `dg` — base-12 dodecagram

---

## 🔧 Variables & Output
```quarter
val x as int: 10
var y as float: 2.5
say "The value is {x}"
```

---

## 🧬 Derivatives
```quarter
val base as int: 20
derive speed from base by 5
say speed  # outputs 25
```

---

## 🧮 DG (Dodecagram) System
```quarter
val hex as dg: 9A1
say from_dg(hex)     # Outputs decimal
say to_dg(1234)      # Converts decimal to DG
```

### DG Math
```quarter
say dg_add(100, 200)
say dg_mul(12, 3)
```

---

## 🔁 Control Flow
### Loop
```quarter
loop from 1 to 5:
  say "Counting..."
```

### Conditional
```quarter
when x > 10:
  say "Large"
else:
  say "Small"
```

### Match Case
```quarter
match val:
  case 1:
    say "One"
  case 2:
    say "Two"
```

---

## 🎯 Functions
```quarter
define greet(name as string):
  say "Hello, {name}"
```

### Return & Yield
```quarter
define double(n as int):
  return n * 2
```

---

## 🧵 Threads & Pipes
```quarter
thread monitor()
pipe write: "logfile.txt"
```

---

## ⚙️ Inline NASM
```quarter
asm {
  mov rax, 7
  inc rax
}
```

---

## 📦 Capsules
Compiled as `.qtrcapsule`:
- `QTRC` magic header
- Version block
- Compressed:
  - Source
  - NASM
  - DG Metadata

```cpp
writeCapsuleWithDG(asmCode, srcCode, dg_meta);
```

---

## 🔧 Runtime Built-ins
| Function      | Description                  |
|---------------|------------------------------|
| `to_dg(n)`    | Int → DG string              |
| `from_dg(d)`  | DG string → Int              |
| `dg_add(a,b)` | Add → DG result              |
| `dg_mul(a,b)` | Multiply → DG result         |

---

## ✅ Example Program
```quarter
star
val velocity as int: 88
derive momentum from velocity by 44
say momentum

val hex as dg: 9A1
say from_dg(hex)
say to_dg(1234)
say dg_add(100, 100)
say dg_mul(12, 12)

loop from 1 to 3:
  say "Quarter Running..."
end
```


---

## 🔗 License
MIT © 2025 Violet Aura Creations  
Follow development: [https://github.com/JoeySoprano420/Quarter-Programming-Language](#)

---

## ✨ Contributions
Open issues, pull requests, and community examples welcome!

# QuarterLang AOT Compiler 🔧

**Readable like English. Powerful like Assembly. Beautiful like poetry.**

This is the official AOT compiler for [QuarterLang](https://github.com/JoeySoprano420/Quarter-Programming-Language), designed to compile `.qtr` source files directly into **Windows x86-64 binaries** using **inline NASM**, **inline hexadecimal IR**, and **DodecaGram (DG)** annotations.

---

## 🧠 Architecture

- **Lexer → Parser → AST → DG IR → NASM + Hex**
- **SIMD-aware DG backend**
- **Inline DG + Hex IR + NASM**
- **Zero-runtime startup**
- **Built-in concurrency, async/await, threads**
- **LLVM IR bridge for hybrid optimization**
- **Massive SIMD-optimized stdlib (math, I/O, crypto, net)**
- **FFI to .NET and WebAssembly**

---

## 📁 Directory Structure

QuarterLang_Compiler/ ├── QuarterLang_MemoryHandler.asm ├── QuarterLang_RangeAdjuster.asm ├── QuarterLang_ErrorHandler.asm ├── QuarterLang_Indexter.asm ├── QuarterLang_IO.asm ├── QuarterLang_Filer.asm ├── QuarterLang_LibrarySystem.asm ├── QuarterLang_Lexer.asm ├── QuarterLang_SyntaxHighlighter.asm ├── QuarterLang_Formatter.asm ├── QuarterLang_Parser.asm ├── QuarterLang_AST.asm ├── QuarterLang_IRBytecode.asm ├── QuarterLang_Encapsulation.asm ├── QuarterLang_Scoper.asm ├── QuarterLang_Binder.asm ├── QuarterLang_Adapter.asm ├── QuarterLang_Composer.asm ├── QuarterLang_Seeder.asm ├── QuarterLang_CodeGenerator.asm ├── QuarterLang_BinaryEmitter.asm ├── QuarterLang_Renderer.asm ├── QuarterLang_Optimizer.asm ├── QuarterLang_Compactor.asm ├── QuarterLang_Inliner.asm ├── QuarterLang_TLCM.asm ├── QuarterLang_Wrapper.asm ├── QuarterLang_Environment.asm ├── QuarterLang_Runtime.asm ├── QuarterLang_Runner.asm ├── QuarterLang_Debugger.asm ├── QuarterLang_PackageManager.asm ├── QuarterLang_Protocol.asm ├── QuarterLang_CodeInjector.asm ├── QuarterLang_SyntaxHighlighter_IDE.asm ├── QuarterLang_CodeCompletionAgent.asm ├── QuarterLang_Detector.asm ├── QuarterLang_Reader.asm ├── QuarterLang_Formatter_UI.asm └── QuarterLang_CICDPipeline.asm



---

## 🧪 Example Output

QuarterLang:
```quarter
say "Hello, Quarter!"
exit


NASM + Hex + DG:
NASM:
mov rax, 1              ; db 0xB8, 0x01, 0x00, 0x00, 0x00   ; [DG:SYSCALL_WRITE]
mov rdi, 1              ; db 0xBF, 0x01, 0x00, 0x00, 0x00   ; [DG:FD_STDOUT]
mov rsi, msg            ; db 0x48, 0x8D, 0x35, ...          ; [DG:PTR_HELLO]
mov rdx, 18             ; db 0xBA, 0x12, 0x00, 0x00, 0x00   ; [DG:LEN_HELLO]
syscall                 ; db 0x0F, 0x05                     ; [DG:SYSCALL]


🛠 Build Instructions:
Bash:
nasm -f win64 QuarterLang_Runner.asm -o runner.obj
link runner.obj /SUBSYSTEM:CONSOLE /OUT:runner.exe


📚 License
MIT © 2025 Violet Aura Creations Inspired by the vision of QuarterLang


✨ Contributions
Pull requests, forks, and DG-enhanced modules welcome!



✅ Unified Runtime System Complete
| Feature               | ✅ Status |
| --------------------- | -------- |
| DG Memory Execution   | ✅ Done   |
| DG Vector Simulation  | ✅ Done   |
| AST Runtime           | ✅ Done   |
| REPL Mode             | ✅ Done   |
| Live Execution Pane   | ✅ Done   |
| IDE ↔ Runtime Binding | ✅ Done   |
| Hot Reload            | ✅ Done   |



