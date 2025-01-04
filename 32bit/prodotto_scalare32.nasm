%include "sseutils32.nasm"

section .data

section .bss
    alignb 16
    z   resd 1

section .text


global prodscal

a equ 8
b equ 12

prodscal:
    start

    mov eax, [ebp + a]
    mov ecx, [ebp + b]
        
    movaps  xmm0, [eax]
    movaps  xmm1, [ecx]

    mulps   xmm0, xmm1
    haddps  xmm0, xmm0
    haddps  xmm0, xmm0
    movss   [z], xmm0
    fld dword [z]
    ;printss z

    stop
