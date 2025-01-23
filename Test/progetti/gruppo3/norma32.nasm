%include "sseutils32.nasm"

section .data

section .bss
    alignb 16
    z   resd 3

section .text


global norma

a equ 8

norma:
    push		ebp		
	mov		ebp, esp	
	push		ebx		
	push		esi
	push		edi


    mov eax, [ebp + a]

    movaps  xmm0, [eax] ; xmm0 contiene il vettore a
    movaps  xmm1, xmm0 ; faccio una copia di xmm0 in xmm1
    mulps   xmm0, xmm0 ; a[i] = a[i] * a[i]
    haddps  xmm0, xmm0 
    haddps  xmm0, xmm0 ; a = a[0] + a[1] + a[2] + a[3]
    sqrtss  xmm0, xmm0 ; norma = sqrt(a)

    shufps  xmm0, xmm0, 0 ; copio il valore finale della norma in tutte le posizioni

    divps  xmm1, xmm0 ; divido il vettore iniziale per la norma
    movaps  [z], xmm1
    lea eax , [z] ; salvo l'indirizzo di z in eax

 
    pop	edi		
	pop	esi
	pop	ebx
	mov	esp, ebp	
	pop	ebp		
	ret