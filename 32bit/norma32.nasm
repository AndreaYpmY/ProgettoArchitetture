%include "sseutils32.nasm"

section .data

section .bss
    alignb 16
    z   resd 3

section .text


global norma

a equ 8

norma:
    push ebp
    mov ebp, esp
    push ebx

    mov eax, [ebp + a]

    movaps  xmm0, [eax]
    movaps  xmm1, xmm0
    mulps   xmm0, xmm0
    haddps  xmm0, xmm0
    haddps  xmm0, xmm0
    sqrtss  xmm0, xmm0

    shufps  xmm0, xmm0, 0

    divps  xmm1, xmm0
    movaps  [z], xmm1
    lea eax , [z]

    pop ebx
    mov esp, ebp
    pop ebp
