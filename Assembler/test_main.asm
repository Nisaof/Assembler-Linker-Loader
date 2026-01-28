; test_main.asm
;
; Main SMPL program.
; Uses external subroutine AD5 and global data XX and ZZ.
; Loop behavior:
;   - Load XX
;   - Call AD5 (uses YY)
;   - Add ZZ and call AD5 again
;   - Store result at absolute address 70
;   - Decrement ZZ until it becomes negative, then branch to EX and halt.
PROG MAIN
EXTREF AD5,XX,ZZ
START
LOOP: LDA XX
      CLL AD5
      ADD ZZ
      CLL AD5
      STA 70
      LDA ZZ
      SUB #1
      BLT EX
      JMP LOOP
EX:   HLT
      END
