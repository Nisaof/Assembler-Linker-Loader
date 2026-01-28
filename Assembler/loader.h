/*
 * loader.h
 *
 * Interface for the relocating loader used in the SMPL toolchain.
 * The loader:
 * - Reads the linked executable `.exe` file and the DAT information from `.t`
 * - Loads bytes into the global memory array `M[]`
 * - Applies relocation based on a user-specified load point
 * - Can print a decoded view of the memory contents.
 */
#ifndef LOADER_H
#define LOADER_H

#include "tables.h"

// Main loader function: reads `.exe` and `.t` (DAT) files and loads them into M[].
void runLoader(const char *exe_file, const char *t_file);

// Print the memory array to stdout between optional start/end addresses.
void printMemoryArray(int start_addr, int end_addr);

// Split an address into two 2-digit strings: 1033 -> "10" "33".
void intToTwoCharString(int value, char *high, char *low);

#endif // LOADER_H

