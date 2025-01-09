%include "sseutils64.nasm"

section .data

section .bss
    alignb 32
    z   resq 3

    alignb 32  
    w   resq 1

section .text


global norma


norma:
    push    rbp				; salva il Base Pointer
	mov		rbp, rsp		; il Base Pointer punta al Record di Attivazione corrente
	pushaq

    vmovapd  ymm0,  [rdi]        ;vettore primo parametro

    vmovapd ymm3,   ymm0

    vmulpd  ymm0,   ymm0
    vhaddpd ymm0,   ymm0
    vperm2f128  ymm1,   ymm0,   ymm0,   00000001b
    addsd   xmm0, xmm1    
    vsqrtsd xmm0, xmm0

    vmovsd [rsi], xmm0          ; valore di ritorno per riferimento

    
    ;shufpd  xmm0, xmm0, 0
    ;vperm2f128  ymm4,   ymm0,   ymm0,   00000000b
    ;vdivpd  ymm3, ymm4

    ;vmovapd [z], ymm3
    ;lea     rdx,    [z]

 
    popaq				; ripristina i registri generali
	mov		rsp, rbp	; ripristina lo Stack Pointer
	pop		rbp		    ; ripristina il Base Pointer
	ret