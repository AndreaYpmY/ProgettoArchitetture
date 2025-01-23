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

    ; vettore di 4 elementi (1 elemento di padding)
    movaps xmm0,  [eax] 

    ; matrice 3x4 elementi (3 elementi di padding)
    movaps xmm1,  [ecx]     
    movaps xmm2,  [ecx+16] 
    movaps xmm3,  [ecx+32]  

    ; moltiplicazione tra tutti gli elementi di xmm0 e xmm1
    mulps xmm1, xmm0
    ; somma di tutti gli elementi di xmm1
    haddps xmm1, xmm1
    haddps xmm1, xmm1
    movss [results], xmm1 ; salvo il primo elemento
    
    ; ripeto il procedimento sia per xmm2 che per xmm3
    mulps xmm2, xmm0
    haddps xmm2, xmm2
    haddps xmm2, xmm2
    movss [results+4], xmm2 ; salvo il secondo elemento


    mulps xmm3, xmm0
    haddps xmm3, xmm3
    haddps xmm3, xmm3
    movss [results+8], xmm3 ; salvo il terzo elemento

    ; carico il riferimento di results in eax
    lea eax, [results]

    pop edi
    pop esi
    pop ebx
    mov esp, ebp
    pop ebp
    ret