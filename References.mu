🧾 Statements & Keywords
|:Keyword:|	|:Purpose:|	|:Example:|
|:val:|	|:Declare immutable variable:|	|:val x as int: 5:|
|:var:|	|:Declare mutable variable:|	|:var score as float: 0.0:|
|:say:|	|:Output a value:|	|:say "hello":|
|:loop:|	|:Create a for-loop:|	|:loop from 1 to 5:|
|:when:|	|:Conditional branch:|	|:when x > 0:|
|:else:|	|:Alternative branch:|	|:else:|
|:derive:|	|:Define a value as a transformation:|	|:derive speed from velocity by 2:|
|:dg:|	|:Declare a DG (dodecagram, base-12):|	|:val x as dg: 9A1:|
|:match:|	|:Match against multiple cases:|	|:match score:|
|:case:|	|:Case in match:|	|:case 100:|
|:define:|	|:Define a function:|	|:define greet():|
|:procedure:|	|:Alternative to define:|	|:procedure run():|
|:return:|	|:Exit function with value:| |:return 5:|
|:yield:|	|:Yield from generator:|	|:yield item:|
|:thread:|	|:Launch in parallel:|	|:thread do_work():|
|:pipe:|	|:Direct output or data flow:|	|:val out as pipe: "output.log":|
|:nest:|	|:Nest or inline encapsulation:|	|:nest x:|
|:fn:|	|:Anonymous function:|	|:fn a b -> a + b:|
|:asm:|	|:Inline assembly block:|	|:asm { mov rax, 5 }:|
|:stop:|	|:Immediate program halt:|	|:stop:|




🔤 Core Grammar & Structure
🪐 Program Blocks
Begin with: star

End with: end

quarter:

star
  ...code...
end



🧠 Expressions & Evaluations
🎯 Arithmetic
quarter:

val result as int: 4 + 2 * 3
🧭 Comparisons
quarter:

when x > 10:
  say "Big"
💬 String Interpolation
quarter:

val name as string: "Alice"
say "Hello, {name}"
🧩 Derivatives
Derive one variable from another:

quarter:

val base as int: 10
derive total from base by 5   # total = base + 5
🧮 DGs (Dodecagrams)
DGs are base-12 values supporting symbolic math.

📌 Literal & Conversion
quarter:

val d as dg: 9A1
say from_dg(d)          # Convert DG to int
say to_dg(2345)         # Convert int to DG
➕ DG Math
quarter
Copy
Edit
say dg_add(100, 20)     # DG addition
say dg_mul(12, 3)       # DG multiplication
DG Digits: 0–9, X (10), Y (11)

🔁 Control Flow
🔂 Loop
quarter:

loop from 1 to 5:
  say "Looping!"
🔢 Match-Case
quarter
Copy
Edit
match value:
  case 1:
    say "One"
  case 2:
    say "Two"
🎭 Procedures & Functions
🧷 Declaration
quarter:

define greet(name as string):
  say "Hi, {name}"
🌀 Return / Yield
quarter:

define square(n as int):
  return n * n
🧵 Threads & Pipes
quarter
Copy
Edit
thread log_task()
pipe write: "log.txt"
⚙️ Inline NASM Assembly
quarter
Copy
Edit
asm {
  mov rax, 7
  inc rax
}
📦 Capsule Format
Compiled .qtrcapsule contains:

Header: "QTRC" + version

Compressed:

NASM output

Source code

DG Metadata block

Runtime auto-extracts, visualizes, and maps capsule contents.

🧠 QuarterOps Runtime
Core function bindings

Function	Description
to_dg(n)	Convert integer → DG string
from_dg(d)	Convert DG string → integer
dg_add(a,b)	Add two integers → DG
dg_mul(a,b)	Multiply two integers → DG

📘 Example: Full Program
quarter:

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

