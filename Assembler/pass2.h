/*
 * pass2.h
 *
 * Public interface for Pass 2 of the SMPL assembler.
 * Pass 2 completes code generation by resolving all forward and external
 * references and writing the final `.o` and `.t` outputs.
 */
#ifndef PASS2_H
#define PASS2_H

void runPass2(const char *s_file, const char *o_file, const char *t_file);

#endif // PASS2_H
