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

