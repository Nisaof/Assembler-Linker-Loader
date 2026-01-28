/*
 * main.c
 *
 * Top-level driver for the SMPL two-pass assembler.
 * - Opens the input `.asm` file
 * - Runs Pass 1 to build all tables and generate partial code (.s)
 * - Runs Pass 2 to produce the final object (.o) and table (.t) files
 * - Prints all internal tables used by the assembler
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "parser.h"
#include "tables.h"
#include "pass1.h"
#include "pass2.h"

// Remove the extension from a file name (if any) and append a new extension.
void getOutputFilename(const char *input_file, const char *extension, char *output_file, size_t size) {
    strncpy(output_file, input_file, size - 1);
    output_file[size - 1] = '\0';
    
    // Find and remove the `.asm` extension if it exists.
    char *dot = strrchr(output_file, '.');
    if (dot != NULL && strcmp(dot, ".asm") == 0) {
        *dot = '\0';
    }
    
    // Append the new extension (e.g., ".s", ".o", ".t").
    strncat(output_file, extension, size - strlen(output_file) - 1);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s <asm_file>\n", argv[0]);
        printf("Example: %s test_main.asm\n", argv[0]);
        return 1;
    }
    
    const char *input_file = argv[1];
    FILE *fp = fopen(input_file, "r");
    if (!fp) {
        printf("ERROR: cannot open input file %s\n", input_file);
        return 1;
    }

    // Build output file names for partial (.s), object (.o) and table (.t) files.
    char s_file[256], o_file[256], t_file[256];
    getOutputFilename(input_file, ".s", s_file, sizeof(s_file));
    getOutputFilename(input_file, ".o", o_file, sizeof(o_file));
    getOutputFilename(input_file, ".t", t_file, sizeof(t_file));

    // Open the `.s` file where Pass 1 will write partial code.
    FILE *sfp = fopen(s_file, "w");
    if (!sfp) {
        printf("ERROR: cannot create output file %s\n", s_file);
        fclose(fp);
        return 1;
    }

    // Initialize all global assembler tables (symbol, forward, DAT, HDRM).
    initTables();

    char line[100];
    ParsedLine pl;
    int LC = 0;
    int isStartFound = 0;


    while (fgets(line, sizeof(line), fp)) {
        // Strip trailing newline, skip empty / whitespace-only lines.
        int len = strlen(line);
        if (len > 0 && line[len-1] == '\n') line[len-1] = '\0';
        
        int res = parseLine(line, &pl);
        if (res <= 0) continue;

        // PROG pseudo-op: only records program name, does not affect LC.
        if (strcmp(pl.opcode, "PROG") == 0) {
            printf("PROG: %s\n", pl.operand);
            // Add H (Header) record for program name into the HDRM table.
            addToHDRMTable('H', pl.operand, 0);
            continue;
        }

        // START pseudo-op: initialize LC and mark that we are inside the program body.
        if (strcmp(pl.opcode, "START") == 0) {
            LC = 0;
            isStartFound = 1;
            printf("START\n");
            continue;
        }

        // END pseudo-op: terminate assembly of this module.
        if (strcmp(pl.opcode, "END") == 0) {
            printf("END\n");
            break;
        }

        // EXTREF and ENTRY pseudo-ops: processed by Pass 1 but do not change LC.
        if (strcmp(pl.opcode, "EXTREF") == 0 || strcmp(pl.opcode, "ENTRY") == 0) {
            printf("%s: %s\n", pl.opcode, pl.operand);
            processPass1(&pl, LC, sfp);
            continue;
        }

        // If we see an instruction before a START directive, skip it with a warning.
        if (!isStartFound) {
            printf("UYARI: START bulunmadan instruction bulundu, atlaniyor\n");
            continue;
        }

        pl.lc = LC;

        // Print parsed line information (for debugging / tracing).
        printf("LC=%3d | Label: %-8s Opcode: %-8s Operand: %s\n",
               pl.lc,
               strlen(pl.label) ? pl.label : "-",
               pl.opcode,
               strlen(pl.operand) ? pl.operand : "-");

        // Pass 1 actions: update all tables and generate partial code into `.s`.
        processPass1(&pl, LC, sfp);

        LC += pl.size;
    }

    fclose(fp);
    fclose(sfp);

    //D kayıtlarının adreslerini güncelle (ENTRY için)
    updateDRecordAddresses();

    //Tabloları yazdır
    printAllTables();

    //Pass 2'yi çalıştır (dosya adlarını parametre olarak geç)
    runPass2(s_file, o_file, t_file);
    return 0;
}
