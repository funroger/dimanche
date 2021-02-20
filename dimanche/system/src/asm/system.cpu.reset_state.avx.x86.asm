; copyright (c) 2013 Victor Cherepanov
; distributed under BSD 3-clause license

section .data

; code section of file
section .text

global di_system_cpu_reset_state_avx_asm

align 10h
di_system_cpu_reset_state_avx_asm:
    emms
    vzeroupper
    ret
