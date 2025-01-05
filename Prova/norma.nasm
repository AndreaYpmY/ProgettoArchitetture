%include "sseutils32.nasm"

section .data
    alignb 16
    zero_mask dd 1.0, 1.0, 1.0, 0.0

section .bss
    alignb 16
    z   resd 3

section .text


global normaasm

a equ 8

normaasm:
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
    movups  [z], xmm1
    lea eax , [z]

    pop ebx
    mov esp, ebp
    pop ebp
    ret

