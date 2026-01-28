/*
 * pass1.h
 *
 * Public interface for Pass 1 of the SMPL assembler.
 * Pass 1 is responsible for:
 * - Building assembler tables (ST, FRT, DAT, HDRM)
 * - Generating partial object code (`.s`) with unresolved addresses marked as "??"
 */
#ifndef PASS1_H
#define PASS1_H

#include "parser.h"
#include "tables.h"

// Perform all Pass 1 actions for a parsed line and emit partial code to `sfp`.
void processPass1(ParsedLine *pl, int LC, FILE *sfp);
void updateSymbolTable(ParsedLine *pl, int LC);
void updateFRT(ParsedLine *pl, int LC);
void updateDAT(ParsedLine *pl, int LC);
void updateHDRM(ParsedLine *pl, int LC);
void generatePartialCode(ParsedLine *pl, int LC, FILE *sfp);

// Convert mnemonic + addressing mode into a 2-hex-digit opcode string.
void opcodeToHex(const char *opcode, const char *operand, char *hexOpcode);

#endif

