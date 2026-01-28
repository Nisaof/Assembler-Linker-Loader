; sub.asm
;
; SMPL subroutine module.
; Exports subroutine AD5 which:
;   - Adds immediate value 5 to the accumulator
;   - Stores the result into external symbol YY
;   - Returns to the caller.
PROG SBR1
ENTRY AD5
EXTREF YY
START
AD5:  ADD #5
      STA YY
      RET
      END
