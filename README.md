🧠 RISC-V Instruction Generator

This C++ project randomly generates RISC-V (RV32I) machine code and its corresponding assembly representation.
It supports all six base instruction formats — R, I, S, B, U, J — and can produce either single-format or mixed-format instruction sets.

Designed for use in simulation tools like Vivado or Logisim, this generator outputs memory-formatted code (e.g., Mem[0] = ...) ready for direct loading into instruction memory arrays.

⸻

🚀 Features
	•	✅ Supports all RV32I base instruction formats:
	•	R-type (e.g., add, sub, and, or, xor, sll, srl, sra)
	•	I-type (e.g., addi, andi, ori, xori, slli, srli, jalr)
	•	S-type (e.g., sb, sh, sw)
	•	B-type (e.g., beq, bne, blt, bge, bltu, bgeu)
	•	U-type (lui, auipc)
	•	J-type (jal)
	•	🔀 Mixed-format generation option (randomly picks among formats per instruction)
	•	💾 Vivado-friendly output (e.g., Mem[i] = binary // assembly)
	•	🧩 Clean, modular design — each format has its own generator function
	•	🌍 Easily extensible for RV32C (Compressed) or custom instruction sets
