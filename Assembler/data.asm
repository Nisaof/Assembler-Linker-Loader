; data.asm
; 
; SMPL data module.
; Defines three global data symbols:
;   XX = 20, YY = 0, ZZ = 3
; These are referenced by the main program and the subroutine module.
PROG DT
START
ENTRY XX,YY,ZZ
XX:   WORD 20
YY:   WORD 0
ZZ:   WORD 3
      END
