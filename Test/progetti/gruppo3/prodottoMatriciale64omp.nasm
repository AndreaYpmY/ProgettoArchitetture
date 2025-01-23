%include "sseutils64.nasm"

section .data


section .bss


section .text
    

global prodmat

prodmat:
    push		rbp				
    mov		rbp, rsp			
    pushaq	


    ; Vettore di 4 elementi (1 di padding)
    vmovapd ymm0, [rdi]

    ; Matrice 3x4 elementi (3 di padding)
    vmovapd ymm1, [rsi]     
    vmovapd ymm2, [rsi+32]   
    vmovapd ymm3, [rsi+64]   

    ; Moltiplicazione tutti gli elementi di ymm0 e ymm1
    vmulpd ymm1, ymm0       
    vhaddpd ymm1, ymm1  
    VPERM2F128 ymm4, ymm1, ymm1,00000001b
    vaddsd xmm1, xmm4 ; somma di tutti gli elementi
    vmovsd [rdx], xmm1 ; salvo il primo elemento in rdx

    ; ripeto il procedimento sia per ymm2 che per ymm3 

    vmulpd ymm2, ymm0
    vhaddpd ymm2, ymm2
    VPERM2F128 ymm5, ymm2, ymm2,00000001b
    vaddsd xmm2, xmm5
    vmovsd [rdx+8], xmm2 

    vmulpd ymm3, ymm0
    vhaddpd ymm3, ymm3
    VPERM2F128 ymm6, ymm3, ymm3,00000001b
    vaddsd xmm3, xmm6
    vmovsd [rdx+16], xmm3 


    popaq				
    mov		rsp, rbp	
    pop		rbp		  
    ret	

