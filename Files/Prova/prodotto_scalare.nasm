%include "sseutils32.nasm"

section .data
    alignb 16
    zero_mask dd 1.0, 1.0, 1.0, 0.0

section .bss
    alignb 16
    z   resd 1

section .text


global prodotto

a equ 8
b equ 12

prodotto:
    push ebp
    mov ebp, esp
    push ebx

    mov eax, [ebp + a]
    mov ecx, [ebp + b]
        
    movaps  xmm0, [eax]
    movaps  xmm1, [ecx]
    
    ;mulps  xmm1, xmm6
    ;mulps  xmm0, xmm6

    mulps   xmm0, xmm1
    haddps  xmm0, xmm0
    haddps  xmm0, xmm0
    movss   [z], xmm0
    fld dword [z]
    ;printss z

    pop ebx
    mov esp, ebp
    pop ebp
