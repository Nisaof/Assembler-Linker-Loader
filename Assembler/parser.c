#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "parser.h"

/*
 * parser.c
 *
 * Implements line-level parsing for SMPL assembly source.
 * Responsibilities:
 * - Split each input line into label, opcode, and operand fields
 * - Recognize pseudo-ops like START/END/PROG/EXTREF/ENTRY
 * - Compute the size of each instruction based on opcode and addressing mode
 *   (immediate vs direct) so that the assembler can advance LC correctly.
 */

typedef struct {
    char opcode[10];
    int size;
} OpcodeInfo;

// Opcode table for the SMPL instruction set.
OpcodeInfo opcodeTable[] = {
    {"ADD", 3}, {"SUB", 3}, {"LDA", 3}, {"STA", 3},
    {"CLL", 3}, {"JMP", 3}, {"BEQ", 3}, {"BGT", 3}, {"BLT", 3},
    {"INC", 1}, {"DEC", 1}, {"RET", 1}, {"HLT", 1},
    {"BYTE", 1}, {"WORD", 1},
    {"START", 0}, {"END", 0}, {"EXTREF", 0}, {"ENTRY", 0}, {"PROG", 0}
};

int opcodeCount = sizeof(opcodeTable) / sizeof(OpcodeInfo);

// Return instruction size (in bytes) for the given opcode/operand.
// For ADD/SUB/LDA: immediate = 2 bytes, direct = 3 bytes.
int getInstructionSize(const char *opcode, const char *operand) {
    // Check if operand uses immediate addressing.
    int isImmediate = (operand && operand[0] == '#');
    
    // Special handling for ADD, SUB, LDA (immediate vs direct).
    if (strcmp(opcode, "ADD") == 0) {
        return isImmediate ? 2 : 3;
    }
    if (strcmp(opcode, "SUB") == 0) {
        return isImmediate ? 2 : 3;
    }
    if (strcmp(opcode, "LDA") == 0) {
        return isImmediate ? 2 : 3;
    }
    
    // For all other opcodes, look up the size in the opcode table.
    for (int i = 0; i < opcodeCount; i++) {
        if (strcmp(opcode, opcodeTable[i].opcode) == 0)
            return opcodeTable[i].size;
    }
    return -1;
}

int parseLine(char *line, ParsedLine *out) {
    char temp[100];
    strcpy(temp, line);

    // Initialize defaults for the parsed line.
    out->label[0] = '\0';
    out->opcode[0] = '\0';
    out->operand[0] = '\0';

    char *token;
    char *rest = temp;

    // First token (may be a label or an opcode).
    token = strtok_r(rest, " \t\r\n", &rest);
    if (!token) return 0; // empty line

    // Check for label (identified by trailing ':').
    if (strchr(token, ':')) {
        token[strlen(token) - 1] = '\0'; // strip trailing ':'
        strcpy(out->label, token);

        token = strtok_r(NULL, " \t\r\n", &rest);
        if (!token) {
            printf("ERROR: missing opcode after label\n");
            return -1;
        }
    }

    // Opcode (mandatory).
    strcpy(out->opcode, token);

    // Operand (if any): keep everything after the opcode on the same line.
    token = strtok_r(NULL, "\r\n", &rest);
    if (token) {
        while (*token == ' ' || *token == '\t') token++;
        strcpy(out->operand, token);
    }

    // Compute instruction size based on opcode and operand.
    out->size = getInstructionSize(out->opcode, out->operand);
    if (out->size == -1) {
        printf("ERROR: unknown opcode -> %s\n", out->opcode);
        return -1;
    }

    return 1;
}
