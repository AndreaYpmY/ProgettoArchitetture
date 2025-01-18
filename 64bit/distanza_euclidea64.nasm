%include "sseutils64.nasm"

section .data
    align 32
    zeromask: dq 0.0, 0.0, 0.0, 0.0

section .bss
    alignb 32
    z   resq 4

    alignb 32  
    w   resq 1

section .text


global dist


dist:
    push    rbp				; salva il Base Pointer
	mov		rbp, rsp		; il Base Pointer punta al Record di Attivazione corrente
	pushaq

    vmovapd  ymm0, [rdi]        ;vettore primo parametro
    vmovapd  ymm1, [rsi]        ;vettore secondo parametro

    ;vblendpd ymm0, ymm0, [zeromask], 8 ;
    ;vblendpd ymm1, ymm1, [zeromask], 8 ;
    
    ;vmovupd [z], ymm0
    ;printpd z, 2
    ;vmovupd [z], ymm1
    ;printpd z, 2
    
    vsubpd  ymm0, ymm1
    vmulpd  ymm0, ymm0
    vhaddpd ymm0, ymm0
    vperm2f128   ymm1, ymm0, ymm0, 00000001b
    vaddsd  xmm0, xmm1    
    vsqrtsd xmm0, xmm0
    
    ;vmovupd [z], ymm0
    ;printpd z, 2
    ;vmovupd [z], ymm1
    ;printpd z, 2
    ;vmovsd [w], xmm0
    ;printsd w
    
    vmovsd [rdx], xmm0          ; valore di ritorno per riferimento

    popaq				; ripristina i registri generali
	mov		rsp, rbp	; ripristina lo Stack Pointer
	pop		rbp		    ; ripristina il Base Pointer
	ret