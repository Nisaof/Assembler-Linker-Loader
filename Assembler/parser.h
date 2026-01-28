#ifndef PARSER_H
#define PARSER_H

/*
 * parser.h
 *
 * Data structures and function prototypes for parsing SMPL assembly lines.
 * The `ParsedLine` struct holds the decoded label, opcode, operand, location
 * counter (LC), and instruction size for a single source line.
 */

typedef struct {
    char label[10];
    char opcode[10];
    char operand[20];
    int lc;
    int size;
} ParsedLine;

// Parse a single source line into label/opcode/operand.
//  1  -> success
//  0  -> empty line / comment
// -1  -> syntax or opcode error
int parseLine(char *line, ParsedLine *out);

// Return instruction size in bytes based on opcode and operand.
int getInstructionSize(const char *opcode, const char *operand);

#endif
