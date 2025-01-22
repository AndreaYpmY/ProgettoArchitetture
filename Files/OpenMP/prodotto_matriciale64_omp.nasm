%include "sseutils64_omp.nasm"

section .data


section .bss


section .text
    

global prodmat

prodmat:
    push		rbp				
    mov		rbp, rsp			
    pushaq	

    ; RDI contiene l'indirizzo del vettore
    ; RSI contiene l'indirizzo della matrice

    vmovapd ymm0, [rdi]

    vmovapd ymm1, [rsi]     
    vmovapd ymm2, [rsi+32]   
    vmovapd ymm3, [rsi+64]   

    vmulpd ymm1, ymm0       
    vhaddpd ymm1, ymm1       
    VPERM2F128 ymm4, ymm1, ymm1,00000001b
    vaddsd xmm1, xmm4
    vmovsd [rdx], xmm1   

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

