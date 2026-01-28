/*
 * loader_main.c
 *
 * Standalone entry point for the relocating loader.
 * Allows running only the loader phase on a previously linked `.exe` file
 * and its corresponding DAT information in `.t`.
 */

#include <stdio.h>
#include "loader.h"

int main(int argc, char **argv) {
    if (argc < 3) {
        printf("Usage: %s <exe_file> <t_file>\n", argv[0]);
        printf("Example: %s exp.exe exp.t\n", argv[0]);
        return 1;
    }
    
    const char *exe_file = argv[1];
    const char *t_file = argv[2];
    
    runLoader(exe_file, t_file);
    
    return 0;
}

