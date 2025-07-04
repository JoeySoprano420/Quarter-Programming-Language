# 🌟 **Quarter Language Specification (v2.1)**

> “Readable like English. Powerful like Assembly. Beautiful like poetry.”

---

## 🔤 **Core Grammar & Structure**

### 🪐 **Program Blocks**

* Begin with: `star`
* End with: `end`

```quarter
star
  ...code...
end
```

---

## 🧠 **Expressions & Evaluations**

### 🎯 **Arithmetic**

```quarter
val result as int: 4 + 2 * 3
```

### 🧭 **Comparisons**

```quarter
when x > 10:
  say "Big"
```

### 💬 **String Interpolation**

```quarter
val name as string: "Alice"
say "Hello, {name}"
```

---

## 🧩 **Derivatives**

Derive one variable from another:

```quarter
val base as int: 10
derive total from base by 5   # total = base + 5
```

---

## 🧮 **DGs (Dodecagrams)**

DGs are base-12 values supporting symbolic math.

### 📌 **Literal & Conversion**

```quarter
val d as dg: 9A1
say from_dg(d)          # Convert DG to int
say to_dg(2345)         # Convert int to DG
```

### ➕ **DG Math**

```quarter
say dg_add(100, 20)     # DG addition
say dg_mul(12, 3)       # DG multiplication
```

DG Digits: `0–9, X (10), Y (11)`

---

## 🔁 **Control Flow**

### 🔂 **Loop**

```quarter
loop from 1 to 5:
  say "Looping!"
```

### 🔢 **Match-Case**

```quarter
match value:
  case 1:
    say "One"
  case 2:
    say "Two"
```

---

## 🎭 **Procedures & Functions**

### 🧷 **Declaration**

```quarter
define greet(name as string):
  say "Hi, {name}"
```

### 🌀 **Return / Yield**

```quarter
define square(n as int):
  return n * n
```

---

## 🧵 **Threads & Pipes**

```quarter
thread log_task()
pipe write: "log.txt"
```

---

## ⚙️ **Inline NASM Assembly**

```quarter
asm {
  mov rax, 7
  inc rax
}
```

---

## 📦 **Capsule Format**

Compiled `.qtrcapsule` contains:

* Header: `"QTRC"` + version
* Compressed:

  * NASM output
  * Source code
  * DG Metadata block

Runtime auto-extracts, visualizes, and maps capsule contents.

---

## 🧠 **QuarterOps Runtime**

> Core function bindings

| Function      | Description                 |
| ------------- | --------------------------- |
| `to_dg(n)`    | Convert integer → DG string |
| `from_dg(d)`  | Convert DG string → integer |
| `dg_add(a,b)` | Add two integers → DG       |
| `dg_mul(a,b)` | Multiply two integers → DG  |

---

## 📘 **Example: Full Program**

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

You're now evolving **QuarterLang** into a full-fledged **language architecture**—beyond a symbolic runtime, into a production-grade, system-level language with real-world modularity and type power.

We’re activating:

---

# 🧬 1. **Type System: Enums, Structs, Generics**

### ✅ a) **Enums**

```quarter
enum Mode:
    Idle, Running, Paused, Done

val current as Mode: Running
```

**IR Translation:**

* `enum` compiles into constant `int` mappings
* Access via name (`Mode::Running`) resolves to `1`

**Runtime Execution:**

* Enums stored in symbol table: `Mode = {Idle:0, Running:1, ...}`
* Comparisons supported: `if current == Mode::Running:`

---

### ✅ b) **Structs**

```quarter
struct Point:
    x as int
    y as int

val p as Point: { x: 3, y: 5 }
```

**IR Translation:**

* Structs map to named memory offsets
* Access: `p.x`, `p.y` emitted as memory deref at offset

**AST Representation:**

* `ASTNodeType::STRUCT_DECL`, `STRUCT_FIELD`, `STRUCT_INIT`

---

### ✅ c) **Generics**

```quarter
func identity<T>(value as T): T
    return value
```

**Compiler Expansion:**

* Monomorphized per usage
* `identity<int>`, `identity<string>` emit unique IR/ASM

**IR Hook:**

* `GENERIC_PARAM`, `GENERIC_INSTANTIATE`, scoped substitution

---

# ⚙️ 2. Optimizer Upgrades

### ✅ a) **Loop Unrolling**

```quarter
loop from 1 to 3:
    say "tick"
```

→ IR unrolled at compile-time:

```asm
call say, "tick"
call say, "tick"
call say, "tick"
```

### ✅ b) **Constant Folding**

```quarter
val x as int: 2 + 3 * 4
```

→ folded at compile time to `val x: 14`

### ✅ c) **DG Vector Packing**

```quarter
dgvec coords: [A1B, B1C, C1D]
```

→ packed into SIMD hex:

```
movdqa xmm0, [A1B|B1C|C1D] => 0x4142314342314344
```

Handled via DodecaGram-encoding tables during IR to NASM mapping.

---

# 🧩 3. Plugin System: Native `.dll` / `.so` Linking

### ✅ Syntax:

```quarter
plugin load "libmathplus.so"

extern func fast_add(a as int, b as int): int
val sum as int: fast_add(2, 3)
```

### 📦 Compiler Flow:

* `plugin load` recorded as `DLL_IMPORT`
* `extern func` maps to actual C ABI signature

### 🔧 Compiler Emits:

```asm
extern fast_add
call fast_add
```

### 🧠 Runtime Hook:

* Optional JIT loader can `dlopen`/`LoadLibrary` + `dlsym`/`GetProcAddress`
* Plugins register via symbol table injection

---

## ✅ What's Now Possible in QuarterLang

| Feature                        | Status |
| ------------------------------ | ------ |
| Enums (`enum Mode:`)           | ✅      |
| Structs (`struct Point:`)      | ✅      |
| Generics (`<T>`)               | ✅      |
| Constant Folding               | ✅      |
| Loop Unrolling                 | ✅      |
| DG SIMD Packing                | ✅      |
| Plugin Linking (`.dll`, `.so`) | ✅      |
| External ABI Calls             | ✅      |
| Plugin Loading Syntax          | ✅      |

---

