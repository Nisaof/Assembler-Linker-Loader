#Assembler-Linker-Loader System

### CSE 232 - Systems Programming Term Project | Fall 2025

This project implements a full **two-pass assembler, linker, and loader system** for the **SMPL (Simple)** assembly language in C on a Linux environment.

---

## 👥 Team & Responsibilities

The project was developed by a 5-member team, each responsible for a specific module:

| Member | GitHub | Role & Responsibility |
| --- | --- | --- |
| **Nisa Of** | [@Nisaof](https://github.com/Nisaof) | Assembler Pass 1 – Parser: Parsing `.asm` files, extracting labels/opcodes/operands, and performing error checks. |
| **Yiğit Can Turan** | [@Yigit1708](https://github.com/Yigit1708) | Assembler Pass 1 – Tables & Partial Code: Managing ST, FRT, DAT, HDRM tables and generating `.s` (partial code) files. |
| **Efe Demirci** | [@efedemirci04](https://github.com/efedemirci04) | Assembler Pass 2: Resolving forward references and producing final `.o` and `.t` files. |
| **Deniz Baltaş** | [@denizbaltas](https://github.com/denizbaltas) | Linker: Building ESTAB, resolving external references, and producing `.exe` files. |
| **Işıl Hocaoğlu** | [@isilhocaoglu](https://github.com/isilhocaoglu) | Loader & Team Leader: Accepting `loadpoint`, performing relocation, memory dumping, and integration. |

---

## 🛠️ System Architecture & Workflow

The system simulates the full pipeline from source code to executable memory:

1. **Assembler (Pass 1 & 2):** Reads `.asm` source files, builds symbol tables, and generates relocatable object code (`.o`).  
2. **Linker:** Combines modules and resolves addresses using `EXTREF` and `ENTRY` directives.  
3. **Loader:** Loads the `.exe` file into the memory array `M[]` according to a user-specified `loadpoint` and applies relocation.

---

## 📋 SMPL Instruction Set Summary

Supported format:  
`[Label:] <Opcode> <Address or Operand>`

| Mnemonic | Opcode | Addressing Mode | Description |
| --- | --- | --- | --- |
| **LDA** | E1 / E2 | Direct / Immediate | Loads into AC register. |
| **STA** | F1 | Direct | Stores AC contents to memory. |
| **CLL** | C1 | Direct | Calls subroutine. |
| **BEQ/BGT/BLT** | B1/B2/B3 | Relative | Conditional branching. |
| **HLT** | FE | Implied | Halts program execution. |

---

## 🔹 Parser Module Details

This module parses `.asm` files line by line and prepares them for the following stages:

* **Line Parsing:** Splits each line into `label / opcode / operand`.
* **Error Checking:** Detects formatting errors or missing operands.
* **Location Counter (LC) Management:** Tracks memory positions of each line.
* **Table Support Functions:** Provides data for Symbol Table (ST) and Forward Reference Table (FRT).  
* **Output:** Supplies data for Pass 1 partial code and `.s` file generation.

Example usage:

```c
char *line = "START LDA XX";
parse_line(line, &label, &opcode, &operand);
printf("Label: %s, Opcode: %s, Operand: %s\n", label, opcode, operand);
```

---

## 📁 Project File Structure

```
├── main.c           # Main entry point
├── parser.c/.h      # Line parsing
├── tables.c/.h      # ST, FRT, DAT, HDRM management
├── pass1.c/.h       # Assembler Pass 1
├── pass2.c/.h       # Assembler Pass 2
├── linker_exec.c    # Linker implementation
├── loader.c/.h      # Loader implementation
├── loader_main.c    # Loader main program
├── Makefile         # Build system
├── test_main.asm    # Test: Main program
├── sub.asm          # Test: Subroutine module
└── data.asm         # Test: Data module
```

---

## 🚀 Build & Run

The project is compatible with Linux using `gcc` and `make`.

```bash
# Build project
make all

# Run the complete pipeline
./main test_program.asm
```

---

## 🖼️ Pipeline Diagram

```
.asm (source files)
      │
      ▼
  Assembler Pass 1
      │ (Parser → ST/FRT/DAT/HDRM)
      ▼
  Assembler Pass 2
      │ (Final .o and .t files)
      ▼
    Linker
      │ (.exe file + updated .t table)
      ▼
    Loader
      │ (Relocation using loadpoint)
      ▼
   Memory Array M[]
```

---

## 💾 Example Memory Dump

Example program loaded at `loadpoint = 1000`:

```
Address   Hex Code
1000      E1 10 33
1003      C1 10 27
1006      A1 10 35
1009      C1 10 27
1012      F1 00 70
1015      E1 10 35
1018      A4 01
1020      B3 10 26
1023      B4 10 00
1026      FE
1027      A2 05
1029      F1 10 34
1032      C2
1033      20
1034      0
1035      3
```

---

## 📅 Project Timeline & Reporting

* **Report 1 (11.12.2025):** Design & Pass 1 details
* **Report 2 (21.12.2025):** Linker & Loader logic
* **Final Submission (05.01.2026):** Fully working system & demo

---

## ✅ Notes

* All modules are integrated and tested to ensure smooth pipeline execution.
* Error handling is included at Pass 1 to prevent propagation of invalid instructions.
* The project follows a modular design, making it easy to extend or adapt to other assembly languages.

