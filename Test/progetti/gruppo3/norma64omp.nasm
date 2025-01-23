%include "sseutils64.nasm"

section .data


section .bss


section .text


global norma


norma:
    push    rbp				; salva il Base Pointer
	mov		rbp, rsp		; il Base Pointer punta al Record di Attivazione corrente
	pushaq

   vmovapd  ymm0,  [rdi] ; ymm0 contiene il vettore a

    vmovapd ymm3,   ymm0 ; faccio una copia di ymm0 in ymm3

    vmulpd  ymm0,   ymm0 ; a[i] = a[i] * a[i]
    vhaddpd ymm0,   ymm0
    vperm2f128  ymm1,   ymm0,   ymm0,   00000001b
    addsd   xmm0, xmm1  ; a = a[0] + a[1] + a[2] + a[3]
    vsqrtsd xmm0, xmm0  ; norma = sqrt(a)
    
    shufpd  xmm0, xmm0, 0 
    vperm2f128  ymm4,   ymm0,   ymm0,   00000000b ; copio il valore finale della norma in tutte le posizioni
    vdivpd  ymm3, ymm4 ; divido il vettore iniziale per la norma

    vmovapd [rsi],    ymm3 ; salvo il vettore normalizzato in rsi

 
    popaq				; ripristina i registri generali
	mov		rsp, rbp	; ripristina lo Stack Pointer
	pop		rbp		    ; ripristina il Base Pointer
	ret