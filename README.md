---
finished_date: 2020-05-27
tags:
  - computer_architecture
  - MIPS
  - Assembly
  - personal_project
  - C
  - cloud_server
  - Makefile
---
# mips_simulator
- MIPS simulator in C

## decoder
read binary file and print corresponding assembly code
- input: binary file of MIPS machine code(big endian)
- output: print corresponding assembly representation
### interface
- simple shell
  - **mips-sim>**
  - accept user line command until program exits
  - supported commands
    - **read [filename]** : read [filename] binary file and print the assembly code of it
    - **exit** : finish the program(finish the shell)

### supported instruction
- logic
  - add, addu, and, div, divu, mult, multu, nor, or, sll, sllv, sra, srav, srl, srlv, sub, subu, xor, addi, addiu, andi,  lui, ori, xori,
- comparison
  - sltiu, slti, slt, sltu
- jump
  - jalr, jr, j, jal
- data movement
  - mfhi, mflo, mthi, mtlo
- branch
  - beq, bne
- load
  - lb, lbu, lh, lhu, lw
- store
  - sb, sh, sw 
- function
  - syscall 
- others
  - unknown instruction

There are some R-type instructions and I-type instructions among supported instructions

### result
```
inst [inst_no]: [original binary code in hex] [assembly inst] 
```
<p align="center">
  <img src="./images/decoder_result.PNG" alt="decoder result"> <br/>
  decoder result
</p>

### filestructure
```
|-- decoder
  |-- main.c
  |-- Makefile
```
- main.c: source code of the project
- Makefile: makefile of the project

### How to run
1. get **main.c, Makefile**
2. enter **make** to prompt to compile 
3. enter **make run** to execute
4. enter **make clean** to erase all generated files
  
## simulator

## 배운점
## 한계점
