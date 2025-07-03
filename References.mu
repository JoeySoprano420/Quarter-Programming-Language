🧾 Statements & Keywords
:Keyword:
:Purpose:
:Example:
val
Declare immutable variable
val x as int: 5
var
Declare mutable variable
var score as float: 0.0
bool
Declare a boolean value
val is_valid as bool: true
truths
Declare foundational truths for logic/proof
truths: identity, motion
proofs
Declare verifiable logical constructs
proofs validate gravity against mass
types
Define or annotate data types
val t as types: numeric
primatives
Declare raw values or low-level types
val id as primative: 42
dodecagrams
Declare DG values (base-12 SIMD symbols)
val id as dodecagram: 9A3
dg
Shorthand for DG declaration
val x as dg: A9B
dgvec
SIMD vector of DodecaGrams
val v as dgvec: [9A1, 9A2, 9A3, 9A4]
loop
Create bounded loop
loop from 1 to 10:
while
Loop while condition is true
while x < 10:
when
Conditional branch
when score > 90:
else
Else branch
else:
elif
Else-if condition
elif score == 80:
stop
Immediate halt of program
stop
match
Pattern match multiple values
match status:
case
Case inside match
case 200:
conditionals
Enable advanced logical structures
conditionals: x > y and y > z
say
Output to console or stdout
say "Running"
define
Named function definition
define compute(x y):
fn
Anonymous inline function
fn a b -> a + b
procedure
Side-effect driven named block
procedure setup()
yield
Yield from coroutine or generator
yield data
return
Return value from function
return result: ok x
thread
Launch parallel thread
thread update_UI()
spawn
Spawn new thread or unit
spawn indexer()
async
Define asynchronous task
async define fetch_data():
await
Await async result
val output: await fetch_data()
lock
Lock shared resource
lock file_handler:
sync
Synchronize threads or scopes
sync:
inline
Inline function or logic
inline multiply()
nest
Create encapsulated block
nest config:
pipe
Direct data stream
val log as pipe: "debug.log"
map
Transform collections
map items with fn x -> x * 2
filter
Filter collection by predicate
filter nums with fn x -> x > 0
reduce
Reduce collection to single result
reduce nums with fn acc x -> acc + x
bind
Bind value or result
bind total to sum(x, y)
derive
Create transformation of value
derive z from x by 2
from
Specify source in derivation/import
derive b from a:
by
Modifier in transformation
derive size from area by 2
entry
Insert key-value into table/map
entry users: 101 => "admin"
table
Declare associative map (DG-backed)
table lookup as map[int, string]:
scope
Create a local isolated block
scope buffer:
decorate
Attach metadata to functions or types
@trace define render()
decorators
Define reusable annotations
decorators: @trace, @inline
class
Define a reusable object blueprint
class Window:
object
Instantiate or reference a class
val main as Window: new()
structs
Define structured record types
struct Point: x as int, y as int
module
Declare self-contained namespace
module geometry:
import
Import external module
import crypto.qtr
include
Include file into current source
include "mathlib.qtr"
textures
Reference graphical assets (GPU, media)
val tex as texture: "skin1.png"
update
Declare update logic loop
procedure update_state():
frame
Single tick unit in rendering or sim loop
frame render():
tick, tickrate
Frame timing & scheduling
tickrate: 60hz
unit
Declare atomic isolated operation
unit ClearMemory:
cycle
Timed loop or system trigger
cycle heartbeat every 100ms:
ref
Reference alias to variable
ref current to settings.main:
mutate
Explicit variable mutation
mutate balance with fn x -> x - 5
mirror
Reflect or observe structure
mirror payload:
lens
View/edit subset of structure
lens position from obj:
nodes
Declare DAG or graph-like node units
node filter_gate:
controls
Master flow-control structures
controls: loop, match, case, thread
keywords
Inspect or print language keywords
say keywords
nasm
Inline NASM injection
nasm { mov rdi, rax }
hex
Inline hexadecimal encoding (DG optimized)
hex: 0x48 0x89 0xC7
asm
Inline assembly block
asm { mov rax, 5 }
profile
Mark block for benchmarking
profile "parser_speed":
assert
Runtime assertion check
assert x == 5
test
Declare unit test
test "math add":
option
Optional value container
val name as option[string]: none
result
Success/error union result
return result: ok 42
error
Return error in union
return result: error "bad op"
try, catch
Handle exceptions
try: ... catch e:
finally
Cleanup block
finally:
guard
Protected block
guard:
track
Monitor value at runtime
track packet_id
override
Override method in class/trait
override define toString():
implements
Satisfy contract/trait
define draw(obj) implements Drawable:
concept
Define behavior constraint
concept Equatable:





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

