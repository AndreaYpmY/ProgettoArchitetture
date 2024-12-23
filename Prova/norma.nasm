%include "sseutils32.nasm"

section .data

section .bss
    alignb 16
    z   resd 1

section .text


global norma

a equ 8
b equ 12

norma:
    start

    mov eax, [ebp + a]

    movaps  xmm0, [eax]
    mulps   xmm0, xmm0
    haddps  xmm0, xmm0
    haddps  xmm0, xmm0
    sqrtss  xmm0, xmm0

    movss   [z], xmm0
    fld dword [z]


    stop
