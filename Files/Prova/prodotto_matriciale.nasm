%include "sseutils32.nasm"

section .data
    align 16
    n       dd 3
    alignb 16
    results dd 0.0 , 0.0 , 0.0 ; vettore di 3 elementi

section .bss
    alignb 16
    vector   resd 3 
    alignb 16
    matrix   resd 9

    index    resd 1
    k        resd 1
    i        resd 1
    j        resd 1
    sumindex resd 1

section .text


global prodmat

a equ 8
b equ 12

prodmat:
    push ebp
    mov ebp, esp
    push ebx

    mov eax, [ebp + a]
    mov ecx, [ebp + b]

    
    ; vettore di 3
    movaps xmm0,  [eax]
    movaps [vector], xmm0
    ; matrice 3x3
    movaps xmm1,  [ecx]
    movaps xmm2,  [ecx+16]
    movaps xmm3,  [ecx+32]
    movaps [matrix], xmm1
    movaps [matrix+16], xmm2
    movaps [matrix+32], xmm3

    xorps xmm0, xmm0 
    xorps xmm1, xmm1
    xorps xmm2, xmm2
    xorps xmm3, xmm3
    
    ; Inizializzazione variabili 
    mov dword[index], 0  
    mov dword[k], 0   
    mov dword[i], 0

    jmp ciclo_esterno



ciclo_esterno:
    mov eax, [i]
    cmp eax, [n] ; i < n
    jge fine_ciclo_esterno


    mov dword [j], 0
    mov dword [index], 0

    call ciclo_interno

    add dword [i], 1
    add dword [k], 1


    jmp ciclo_esterno

fine_ciclo_esterno:
    ; finito il ciclo esterno, salvo il risultato
    lea eax, [results]

    pop ebx
    mov esp, ebp
    pop ebp
    ret

ciclo_interno:
    mov ecx, [j]
    cmp ecx, [n] ; j < n
    jge fine_ciclo_interno

    ; calcolo l'indice della matrice
    mov ecx , [index]
    add ecx , [k]
    add ecx , [j]
    mov dword [sumindex], ecx     

    mov ecx , [j]
    movss xmm5, [vector + ecx * 4]

    mov edx, [sumindex]
    movss xmm6, [matrix + edx * 4]

    mulps xmm5, xmm6

    mov ecx, [i]
    movss xmm6, [results + ecx*4]   
    addss xmm6, xmm5                
    movss [results + ecx*4], xmm6    


    add dword [j], 1
    add dword [index], 2

    jmp ciclo_interno
    
fine_ciclo_interno:
    ret