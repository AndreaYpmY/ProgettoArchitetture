%include "sseutils64.nasm"

section .data
    align 32
    zeromask: dq 0.0, 0.0, 0.0, 0.0

section .bss
    alignb 32
    ris resq 3

    alignb 32
    z resq 4

    alignb 32
    h resq 1

section .text
    

global prodmat

prodmat:
    push		rbp				
    mov		rbp, rsp			
    pushaq	

    ; RDI contiene l'indirizzo del vettore
    ; RSI contiene l'indirizzo della matrice

    vmovapd ymm0, [rdi]
    ;vblendpd ymm0, ymm0, [zeromask], 8 ;

    
    vmovapd ymm1, [rsi]     
    vmovapd ymm2, [rsi+32]   
    vmovapd ymm3, [rsi+64]   

    vmulpd ymm1, ymm0       
    vhaddpd ymm1, ymm1       
    VPERM2F128 ymm4, ymm1, ymm1,00000001b
    vaddsd xmm1, xmm4
    vmovsd [rdx], xmm1
    ;vmovsd [ris], xmm1   

    vmulpd ymm2, ymm0
    vhaddpd ymm2, ymm2
    VPERM2F128 ymm5, ymm2, ymm2,00000001b
    vaddsd xmm2, xmm5
    vmovsd [rdx+8], xmm2
    ;vmovsd [ris+8], xmm2 

    vmulpd ymm3, ymm0
    vhaddpd ymm3, ymm3
    VPERM2F128 ymm6, ymm3, ymm3,00000001b
    vaddsd xmm3, xmm6
    vmovsd [rdx+16], xmm3 
    ;vmovsd [ris+16], xmm3


    ;lea rdx, [ris]

    popaq				
    mov		rsp, rbp	
    pop		rbp		  
    ret	

