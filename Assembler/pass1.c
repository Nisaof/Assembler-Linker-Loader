/*
 * pass1.c
 *
 * Implementation of Pass 1 of the SMPL two-pass assembler.
 * Responsibilities:
 * - Map mnemonics and addressing modes to machine opcodes
 * - Build the Symbol Table (ST), Forward Reference Table (FRT),
 *   Direct Address Table (DAT), and HDRM (H/D/R/M) records
 * - Generate partial object code (.s) with placeholders (??) for
 *   unresolved addresses that will be filled during Pass 2.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "pass1.h"

// Opcode mapping table: mnemonic + addressing mode -> hex opcode and size.
typedef struct {
    char mnemonic[10];
    char opcode[3];
    int bytes;
    int isImmediate;
} OpcodeEntry;

OpcodeEntry opcodeMap[] = {
    {"ADD", "A1", 3, 0}, {"ADD", "A2", 2, 1},
    {"SUB", "A3", 3, 0}, {"SUB", "A4", 2, 1},
    {"LDA", "E1", 3, 0}, {"LDA", "E2", 2, 1},
    {"STA", "F1", 3, 0},
    {"CLL", "C1", 3, 0},
    {"JMP", "B4", 3, 0},
    {"BEQ", "B1", 3, 0},
    {"BGT", "B2", 3, 0},
    {"BLT", "B3", 3, 0},
    {"INC", "D2", 1, 0},
    {"DEC", "D1", 1, 0},
    {"RET", "C2", 1, 0},
    {"HLT", "FE", 1, 0}
};

int opcodeMapSize = sizeof(opcodeMap) / sizeof(OpcodeEntry);

// Check whether the given opcode uses relative addressing (for branches).
int isRelativeAddressing(const char *opcode) {
    return (strcmp(opcode, "BEQ") == 0 || 
            strcmp(opcode, "BGT") == 0 || 
            strcmp(opcode, "BLT") == 0);
}

// Map mnemonic + addressing mode to a 2-hex-digit opcode string.
void opcodeToHex(const char *opcode, const char *operand, char *hexOpcode) {
    int isImmediate = (operand && operand[0] == '#');
    
    for (int i = 0; i < opcodeMapSize; i++) {
        if (strcmp(opcodeMap[i].mnemonic, opcode) == 0) {
            if (opcodeMap[i].isImmediate == isImmediate || opcodeMap[i].bytes == 1) {
                strcpy(hexOpcode, opcodeMap[i].opcode);
                return;
            }
        }
    }
    strcpy(hexOpcode, "??"); //Bulunamadı
}

// Return 1 if the operand is numeric (immediate or direct number), 0 otherwise.
int isNumeric(const char *str) {
    if (!str || strlen(str) == 0) return 0;
    if (str[0] == '#') {
        // Immediate constant: e.g., "#5"
        for (int i = 1; str[i]; i++) {
            if (!isdigit(str[i])) return 0;
        }
        return 1;
    }
    // Direct numeric address: e.g., "10", "70"
    for (int i = 0; str[i]; i++) {
        if (!isdigit(str[i])) return 0;
    }
    return 1;
}

// Extract integer value from an operand (handles immediate "#n" and plain "n").
int getNumericValue(const char *operand) {
    if (!operand) return 0;
    if (operand[0] == '#') {
        return atoi(operand + 1);
    }
    return atoi(operand);
}

// Update Symbol Table (ST) with a label defined at the current LC.
void updateSymbolTable(ParsedLine *pl, int LC) {
    // If there is a label, add it to the Symbol Table.
    if (strlen(pl->label) > 0) {
        int existing = findInSymbolTable(pl->label);
        if (existing == -1) {
            // New symbol definition.
            addToSymbolTable(pl->label, LC);
        } else {
            // Symbol is already defined; this may indicate a programming error.
            printf("WARNING: Symbol '%s' already defined (old: %d, new: %d)\n", 
                   pl->label, existing, LC);
        }
    }
}

// Update Forward Reference Table (FRT) for unresolved symbol operands.
void updateFRT(ParsedLine *pl, int LC) {
    // If we see a symbol operand that is not yet defined, record it in FRT.
    if (strlen(pl->operand) > 0 && !isNumeric(pl->operand)) {
        // Only for non-immediate operands (do not treat "#5" as a symbol).
        if (pl->operand[0] != '#') {
            // If it is not an external reference and not in ST yet, it's a forward ref.
            if (!isExternalReference(pl->operand)) {
                int found = findInSymbolTable(pl->operand);
                if (found == -1) {
                    // Symbol not defined yet: record address of its 2-byte field (LC+1 for 3-byte).
                    int addrOffset = (pl->size == 3) ? 1 : 0;
                    addToForwardRefTable(LC + addrOffset, pl->operand);
                }
            }
        }
    }
}

// Update Direct Address Table (DAT) for instructions that use direct numeric addresses.
void updateDAT(ParsedLine *pl, int LC) {
    (void)LC; // currently not used here
    // BYTE/WORD are pure data; they do not go into DAT.
    if (strcmp(pl->opcode, "BYTE") == 0 || strcmp(pl->opcode, "WORD") == 0) {
        return;
    }
    
    // For instructions using direct addressing: if the operand is a numeric
    // address (e.g., STA 70, STA 10), add it into the DAT.
    if (strlen(pl->operand) > 0 && isNumeric(pl->operand) && pl->operand[0] != '#') {
        int addr = getNumericValue(pl->operand);
        addToDirectAdrTable(addr);
    }
}

// Update HDRM table with H/D/R/M records based on pseudo-ops and operand types.
void updateHDRM(ParsedLine *pl, int LC) {
    if (strcmp(pl->opcode, "EXTREF") == 0) {
        // EXTREF: generate R (Reference) records for each external symbol.
        char *operand_copy = strdup(pl->operand);
        char *token = strtok(operand_copy, ",");
        while (token != NULL) {
            while (*token == ' ' || *token == '\t') token++;
            addToHDRMTable('R', token, 0);
            token = strtok(NULL, ",");
        }
        free(operand_copy);
    }
    
    if (strcmp(pl->opcode, "ENTRY") == 0) {
        // ENTRY: generate D (Definition) records (addresses updated at end of Pass 1).
        char *operand_copy = strdup(pl->operand);
        char *token = strtok(operand_copy, ",");
        while (token != NULL) {
            while (*token == ' ' || *token == '\t') token++;
            int addr = findInSymbolTable(token);
            if (addr == -1) addr = 0; //Henüz bilinmiyor
            addToHDRMTable('D', token, addr);
            token = strtok(NULL, ",");
        }
        free(operand_copy);
    }
    
    // For 3-byte instructions, if operand is a symbol (non-numeric, non-immediate),
    // we need an M record for later relocation in the linker.
    if (pl->size == 3 && strlen(pl->operand) > 0) {
        if (!isNumeric(pl->operand) && pl->operand[0] != '#') {
            addToHDRMTable('M', pl->operand, LC + 1);
        }
    }
}

// Generate partial `.s` code for this line at the given LC.
void generatePartialCode(ParsedLine *pl, int LC, FILE *sfp) {
    if (!sfp) return;
    
    // BYTE directive: one data byte.
    if (strcmp(pl->opcode, "BYTE") == 0) {
        int val = getNumericValue(pl->operand);
        fprintf(sfp, "%d %02X\n", LC, val & 0xFF);
        return;
    }
    
    // WORD directive: one data word (currently stored as low byte).
    if (strcmp(pl->opcode, "WORD") == 0) {
        int val = getNumericValue(pl->operand);
        fprintf(sfp, "%d %02X\n", LC, val & 0xFF);
        return;
    }
    
    char hexOpcode[3];
    opcodeToHex(pl->opcode, pl->operand, hexOpcode);
    
    fprintf(sfp, "%d %s ", LC, hexOpcode);
    
    // Emit partial code for the operand portion.
    if (pl->size == 1) {
        fprintf(sfp, "\n");
    } else if (pl->size == 2) {
        if (pl->operand[0] == '#') {
            int val = getNumericValue(pl->operand);
            fprintf(sfp, "%02X\n", val & 0xFF);
        } else {
            fprintf(sfp, "??\n");
        }
    } else if (pl->size == 3) {
        if (isNumeric(pl->operand) && pl->operand[0] != '#') {
            int addr = getNumericValue(pl->operand);
            
            if (isRelativeAddressing(pl->opcode)) {
                int displacement = addr - (LC + 3);
                fprintf(sfp, "%02X %02X\n", (displacement >> 8) & 0xFF, displacement & 0xFF);
            } else {
                fprintf(sfp, "%02X %02X\n", (addr >> 8) & 0xFF, addr & 0xFF);
            }
        } else if (pl->operand[0] == '#') {
            fprintf(sfp, "?? ??\n");
        } else {
            int symbolAddr = findInSymbolTable(pl->operand);
            if (symbolAddr != -1) {
                if (isRelativeAddressing(pl->opcode)) {
                    int displacement = symbolAddr - (LC + 3);
                    fprintf(sfp, "%02X %02X\n", (displacement >> 8) & 0xFF, displacement & 0xFF);
                } else {
                    fprintf(sfp, "%02X %02X\n", (symbolAddr >> 8) & 0xFF, symbolAddr & 0xFF);
                }
            } else {
                fprintf(sfp, "?? ??\n");
            }
        }
    } else {
        fprintf(sfp, "\n");
    }
}

void processPass1(ParsedLine *pl, int LC, FILE *sfp) {
    if (strcmp(pl->opcode, "EXTREF") == 0 || strcmp(pl->opcode, "ENTRY") == 0) {
        updateHDRM(pl, LC);
        return;
    }
    
    updateSymbolTable(pl, LC);
    updateFRT(pl, LC);
    updateDAT(pl, LC);
    updateHDRM(pl, LC);
    
    generatePartialCode(pl, LC, sfp);
}

