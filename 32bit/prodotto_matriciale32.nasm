%include "sseutils32.nasm"

section .data
    alignb 16
    zero_mask dd 1.0, 1.0, 1.0, 0.0
    
section .bss
    alignb 16
    results   resd 3

section .text

global prodmat

a equ 8
b equ 12

prodmat:
    push ebp
    mov ebp, esp
    push ebx
    push esi
    push edi

    mov eax, [ebp + a]
    mov ecx, [ebp + b]

    ; carica zero_mask in xmm6
    movaps xmm6, [zero_mask]

    ; vettore di 3
    movaps xmm0,  [eax]

    ; matrice 3x3
    movaps xmm1,  [ecx]
    mulps xmm1, xmm6
    movups xmm2,  [ecx+12]
    mulps xmm2, xmm6
    movups xmm3,  [ecx+24]
    mulps xmm3, xmm6

    ; moltiplicazione tra tutti gli elementi di xmm0 e xmm1
    mulps xmm1, xmm0
    ; somma di tutti gli elementi di xmm1
    haddps xmm1, xmm1
    haddps xmm1, xmm1
    movss [results], xmm1
    
    ; ripeto il procedimento sia per xmm2 che per xmm3
    mulps xmm2, xmm0
    haddps xmm2, xmm2
    haddps xmm2, xmm2
    movss [results+4], xmm2

    mulps xmm3, xmm0
    haddps xmm3, xmm3
    haddps xmm3, xmm3
    movss [results+8], xmm3

    ; carico il riferimento di results in eax
    lea eax, [results]

    pop edi
    pop esi
    pop ebx
    mov esp, ebp
    pop ebp
    ret