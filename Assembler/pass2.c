/*
 * pass2.c
 *
 * Implementation of Pass 2 of the SMPL two-pass assembler.
 * Responsibilities:
 * - Read the partial code file `.s` produced by Pass 1
 * - Resolve all forward references using FRT, ST and HDRM tables
 * - Fill in any "??" operand bytes with the final symbol addresses
 * - Emit the final relocatable object code `.o` and a `.t` file containing
 *   SYMBOL, FORWARD REFERENCE, DIRECT ADDRESS (DAT) and HDRM tables.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pass2.h"
#include "tables.h" 

// Look for an M record in the HDRM table at the given address (LC+1).
char* findInHDRM_M_Record(int address) {
    for (int i = 0; i < HDRM_count; i++) {
        if (HDRMT[i].code == 'M' && HDRMT[i].address == address) {
            return HDRMT[i].symbol;
        }
    }
    return NULL;
}


void runPass2(const char *s_file, const char *o_file, const char *t_file) {
    // Open input (.s) and output (.o) files.
    FILE *inFile = fopen(s_file, "r");
    FILE *outFile = fopen(o_file, "w");
    
    if (!inFile || !outFile) {
        printf("ERROR: could not open Pass 2 input/output files!\n");
        return;
    }


    char line[128];
    
    // Main loop: read and process each line of partial code.
    while (fgets(line, sizeof(line), inFile)) {
        line[strcspn(line, "\r\n")] = 0; // strip trailing newline
        if (strlen(line) < 2) continue;

        int lc;
        char opcode[5], op1[5], op2[5];
        int tokenCount = 0;

        // Reset temporary buffers and parse the line.
        opcode[0] = '\0'; op1[0] = '\0'; op2[0] = '\0';
        tokenCount = sscanf(line, "%d %s %s %s", &lc, opcode, op1, op2);

        if (tokenCount < 2) continue; 

        // Always write LC and opcode to the output.
        fprintf(outFile, "%d %s ", lc, opcode);

        // If there is no operand (just opcode), finish the line.
        if (tokenCount == 2) {
            fprintf(outFile, "\n");
            continue;
        }

        // If operand is "??", we must resolve it using ST/FRT/HDRM.
        if (strcmp(op1, "??") == 0) {
            int lookupAddr = lc + 1;
            char *symbolName = NULL;
            int isExternal = 0;

            // First search FRT, then HDRM (M) records.
            for (int i = 0; i < FRT_count; i++) {
                if (FRT[i].address == lookupAddr) {
                    symbolName = FRT[i].symbol;
                    break;
                }
            }
            if (symbolName == NULL) {
                symbolName = findInHDRM_M_Record(lookupAddr);
                if (symbolName != NULL) isExternal = 1;
            }

            if (symbolName != NULL) {
                if (isExternal) {
                    fprintf(outFile, "?? ??\n");
                }
                else {
                    int symbolAddr = findInSymbolTable(symbolName);
                    if (symbolAddr != -1) {
                        fprintf(outFile, "%02X %02X\n", (symbolAddr >> 8) & 0xFF, symbolAddr & 0xFF);
                    } else {
                        fprintf(outFile, "?? ??\n"); // error: symbol not found in ST
                    }
                }
            } else {
                fprintf(outFile, "?? ??\n"); // error: reference not found in FRT/HDRM
            }
        } 
        else {
            fprintf(outFile, "%s", op1);
            if (tokenCount > 3) fprintf(outFile, " %s", op2);
            fprintf(outFile, "\n");
        }
    }

    fclose(inFile);
    fclose(outFile);
    
    FILE *tfp = fopen(t_file, "w");
    if (tfp) {
        //1. SYMBOL TABLE
        fprintf(tfp, "=== SYMBOL TABLE ===\n");
        for (int i = 0; i < ST_count; i++) {
            fprintf(tfp, "Symbol: %-10s Address: %d\n", ST[i].symbol, ST[i].address);
        }
        
        //2. FORWARD REFERENCE TABLE
        fprintf(tfp, "\n=== FORWARD REFERENCE TABLE ===\n");
        for (int i = 0; i < FRT_count; i++) {
            fprintf(tfp, "Address: %d Symbol: %s\n", FRT[i].address, FRT[i].symbol);
        }
        
        //3. DIRECT ADDRESS TABLE (DAT)
        fprintf(tfp, "=== DIRECT ADDRESS TABLE (DAT) ===\n");
        for (int i = 0; i < DAT_count; i++) {
            fprintf(tfp, "%d\n", DAT[i].address);
        }
        
        //4. HDRM TABLE
        fprintf(tfp, "\n=== HDRM TABLE ===\n");
        for (int i = 0; i < HDRM_count; i++) {
            fprintf(tfp, "%c %s %d\n", HDRMT[i].code, HDRMT[i].symbol, HDRMT[i].address);
        }
        fclose(tfp);
    }
}
