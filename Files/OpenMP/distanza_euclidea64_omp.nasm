%include "sseutils64_omp.nasm"

section .data


section .bss


section .text


global dist


dist:
    push    rbp				; salva il Base Pointer
	mov		rbp, rsp		; il Base Pointer punta al Record di Attivazione corrente
	pushaq


    vmovapd  ymm0, [rdi]        ;vettore primo parametro
    vmovapd  ymm1, [rsi]        ;vettore secondo parametro
    
    vsubpd  ymm0, ymm1
    vmulpd  ymm0, ymm0
    vhaddpd ymm0, ymm0
    vperm2f128   ymm1, ymm0, ymm0, 00000001b
    vaddsd  xmm0, xmm1    
    vsqrtsd xmm0, xmm0
    
    vmovsd [rdx], xmm0          ; valore di ritorno per riferimento


    popaq				; ripristina i registri generali
	mov		rsp, rbp	; ripristina lo Stack Pointer
	pop		rbp		    ; ripristina il Base Pointer
	ret