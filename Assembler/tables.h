/*
 * tables.h
 *
 * Shared data structures and function prototypes for all assembler/linker/loader
 * tables:
 * - Symbol Table (ST)
 * - Forward Reference Table (FRT)
 * - Direct Address Table (DAT)
 * - HDRM Table (H, D, R, M records)
 * Also defines the `Memory` structure used by the loader.
 */

#include "parser.h"

// Core table structures.
struct SymbolTable {
    char symbol[10];
    int address;
};

struct ForwardRefTable {
    int address;
    char symbol[10];
};

struct DirectAdrTable {
    int address;
};

struct HDRMTable {
    char code;  // H, D, R, or M
    char symbol[10];
    int address;
};

struct Memory {
    int address;
    char symbol[3]; // two characters + '\0'
};

// Global table limits.
#define MAX_ST 10
#define MAX_FRT 20
#define MAX_DAT 30
#define MAX_HDRM 20
#define MEMORY_SIZE 500   // memory array size for loader
#define MAX_OPCODE_LEN 10 // maximum opcode length

extern struct SymbolTable ST[MAX_ST];
extern struct ForwardRefTable FRT[MAX_FRT];
extern struct DirectAdrTable DAT[MAX_DAT];
extern struct HDRMTable HDRMT[MAX_HDRM];
extern struct Memory M[MEMORY_SIZE]; // memory array for loader

extern int ST_count;
extern int FRT_count;
extern int DAT_count;
extern int HDRM_count;

// Function prototypes for table operations.
void initTables(void);
int addToSymbolTable(const char *symbol, int address);
int findInSymbolTable(const char *symbol);
int isExternalReference(const char *symbol); // check if symbol comes from EXTREF
int addToForwardRefTable(int address, const char *symbol);
int addToDirectAdrTable(int address);
int addToHDRMTable(char code, const char *symbol, int address);
void updateDRecordAddresses(void); // update D record addresses from Symbol Table
void printAllTables(void);

#endif

