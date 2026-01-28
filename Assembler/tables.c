/*
 * tables.c
 *
 * Implements the global assembler/linker/loader tables:
 * - Symbol Table (ST)
 * - Forward Reference Table (FRT)
 * - Direct Address Table (DAT)
 * - HDRM Table (H/D/R/M records)
 * Also provides a simple memory array `M[]` used by the loader, and
 * helper functions to initialize, update, and print all of these tables.
 */

#include <stdio.h>
#include <string.h>
#include "tables.h"

// Global tables shared across assembler, linker and loader.
struct SymbolTable ST[MAX_ST];
struct ForwardRefTable FRT[MAX_FRT];
struct DirectAdrTable DAT[MAX_DAT];
struct HDRMTable HDRMT[MAX_HDRM];
struct Memory M[MEMORY_SIZE]; // memory array used by the loader

int ST_count = 0;
int FRT_count = 0;
int DAT_count = 0;
int HDRM_count = 0;

void initTables(void) {
    ST_count = 0;
    FRT_count = 0;
    DAT_count = 0;
    HDRM_count = 0;
}

int addToSymbolTable(const char *symbol, int address) {
    if (ST_count >= MAX_ST) {
        printf("ERROR: Symbol Table is full!\n");
        return -1;
    }
    
    // Check if the symbol is already present.
    for (int i = 0; i < ST_count; i++) {
        if (strcmp(ST[i].symbol, symbol) == 0) {
            printf("ERROR: Symbol '%s' already defined!\n", symbol);
            return -1;
        }
    }
    
    strcpy(ST[ST_count].symbol, symbol);
    ST[ST_count].address = address;
    ST_count++;
    return 0;
}

int findInSymbolTable(const char *symbol) {
    for (int i = 0; i < ST_count; i++) {
        if (strcmp(ST[i].symbol, symbol) == 0) {
            return ST[i].address;
        }
    }
    return -1; // not found
}

int isExternalReference(const char *symbol) {
    // Check in the HDRM table for records with code 'R' (external reference).
    for (int i = 0; i < HDRM_count; i++) {
        if (HDRMT[i].code == 'R' && strcmp(HDRMT[i].symbol, symbol) == 0) {
            return 1; // external reference
        }
    }
    return 0; // not an external reference
}

int addToForwardRefTable(int address, const char *symbol) {
    if (FRT_count >= MAX_FRT) {
        printf("ERROR: Forward Reference Table is full!\n");
        return -1;
    }
    
    FRT[FRT_count].address = address;
    strcpy(FRT[FRT_count].symbol, symbol);
    FRT_count++;
    return 0;
}

int addToDirectAdrTable(int address) {
    if (DAT_count >= MAX_DAT) {
        printf("ERROR: Direct Address Table is full!\n");
        return -1;
    }
    
    // Check if this address is already recorded.
    for (int i = 0; i < DAT_count; i++) {
        if (DAT[i].address == address) {
            return 0; //Zaten var
        }
    }
    
    DAT[DAT_count].address = address;
    DAT_count++;
    return 0;
}

int addToHDRMTable(char code, const char *symbol, int address) {
    if (HDRM_count >= MAX_HDRM) {
        printf("ERROR: HDRM Table is full!\n");
        return -1;
    }
    
    HDRMT[HDRM_count].code = code;
    strcpy(HDRMT[HDRM_count].symbol, symbol);
    HDRMT[HDRM_count].address = address;
    HDRM_count++;
    return 0;
}

// Update D record addresses from the Symbol Table (called at the end of Pass 1).
void updateDRecordAddresses(void) {
    for (int i = 0; i < HDRM_count; i++) {
        if (HDRMT[i].code == 'D') {
            int addr = findInSymbolTable(HDRMT[i].symbol);
            if (addr != -1) {
                HDRMT[i].address = addr;
            }
        }
    }
}

void printAllTables(void) {
    printf("\n=== SYMBOL TABLE ===\n");
    for (int i = 0; i < ST_count; i++) {
        printf("Symbol: %-10s Address: %d\n", ST[i].symbol, ST[i].address);
    }
    
    printf("\n=== FORWARD REFERENCE TABLE ===\n");
    for (int i = 0; i < FRT_count; i++) {
        printf("Address: %d Symbol: %s\n", FRT[i].address, FRT[i].symbol);
    }
    
    printf("\n=== DIRECT ADDRESS TABLE ===\n");
    for (int i = 0; i < DAT_count; i++) {
        printf("Address: %d\n", DAT[i].address);
    }
    
    printf("\n=== HDRM TABLE ===\n");
    for (int i = 0; i < HDRM_count; i++) {
        printf("Code: %c Symbol: %-10s Address: %d\n", 
               HDRMT[i].code, HDRMT[i].symbol, HDRMT[i].address);
    }
}

