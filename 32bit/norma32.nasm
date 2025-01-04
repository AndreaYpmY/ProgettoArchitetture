%include "sseutils32.nasm"

section .data

section .bss
    alignb 16
    z   resd 3

section .text


global norma

a equ 8

norma:
    start

    mov eax, [ebp + a]

    movaps  xmm0, [eax]
    movaps  xmm1, xmm0
    mulps   xmm0, xmm0
    haddps  xmm0, xmm0
    haddps  xmm0, xmm0
    sqrtss  xmm0, xmm0

    divps  xmm1, xmm0
    movaps  [z], xmm1
    lea eax , [z]

    stop
