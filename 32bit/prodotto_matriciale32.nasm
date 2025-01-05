%include "sseutils32.nasm"

section .data



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

    ; vettore di 3
    movaps xmm0,  [eax]

    ; matrice 3x3
    movaps xmm1,  [ecx]
    movaps xmm2,  [ecx+16]
    movaps xmm3,  [ecx+32]

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