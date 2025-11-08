# 🧠 RISC-V Instruction Generator

A **C++ project** that randomly generates **RISC-V (RV32I)** machine code along with its corresponding **assembly representation**.  
It supports all six base **instruction formats** — **R, I, S, B, U, and J** — and can produce either **single-format** or **mixed-format** instruction sets.

Designed for use in **simulation tools** such as **Vivado** or **Logisim**, this generator outputs **memory-formatted code** (e.g., `Mem[0] = ...`) ready for direct loading into instruction memory arrays.

---

## 🚀 Features

### 🧩 Full RV32I Format Support
#### **R-type Instructions**
`add`, `sub`, `sll`, `slt`, `sltu`, `xor`, `srl`, `sra`, `or`, `and`

#### **I-type Instructions**
`addi`, `slti`, `sltiu`, `xori`, `ori`, `andi`,  
`lb`, `lh`, `lw`, `lbu`, `lhu`,  
`slli`, `srli`, `srai`,  
`jalr`

#### **S-type Instructions**
`sb`, `sh`, `sw`

#### **B-type Instructions**
`beq`, `bne`, `blt`, `bge`, `bltu`, `bgeu`

#### **U-type Instructions**
`lui`, `auipc`

#### **J-type Instructions**
`jal`

#### **SYS-type Instructions**
`ecall`, `ebreak`, `fence`, `fence.tso`, `pause`

---


### 🎲 Generation Modes
- **Single-format mode** – generate instructions of one chosen format  
- **Mixed-format mode** – randomly choose among all formats per instruction  

### 💾 Output
- **Vivado-friendly format:**
  ```verilog
  mem[0] = 32'b00000000000100000000000010010011; // addi x1, x0, 1
### ⚙️ Future Improvments

- 🧠 Add RV32C compressed instruction support

- 💻 Add CLI options for:

-  Output format (binary, hex, verilog)


  
