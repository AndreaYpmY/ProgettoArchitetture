%include "sseutils64.nasm"

section .data


section .bss


section .text


global dist


dist:
    push    rbp				; salva il Base Pointer
	mov		rbp, rsp		; il Base Pointer punta al Record di Attivazione corrente
	pushaq


    vmovapd  ymm0, [rdi] ; ymm0 contiene il primo vettore a
    vmovapd  ymm1, [rsi] ; ymm1 contiene il secondo vettore b
    
    vsubpd  ymm0, ymm1 ; a[i] = a[i] - b[i]
    vmulpd  ymm0, ymm0 ; a[i] = a[i] * a[i]
    vhaddpd ymm0, ymm0 
    vperm2f128   ymm1, ymm0, ymm0, 00000001b
    vaddsd  xmm0, xmm1 ; a = a[0] + a[1] + a[2] + a[3] 
    vsqrtsd xmm0, xmm0  ; dista = sqrt(a)
    
    vmovsd [rdx], xmm0 ; valore di ritorno per riferimento


    popaq				; ripristina i registri generali
	mov		rsp, rbp	; ripristina lo Stack Pointer
	pop		rbp		    ; ripristina il Base Pointer
	ret