%include "sseutils32.nasm"

section .data

section .bss

section .text


global dist

a equ 8
b equ 12
dista equ 16

dist:
    push		ebp		
	mov		ebp, esp	
	push		ebx		
	push		esi
	push		edi

    mov eax, [ebp + a] 
    mov ecx, [ebp + b] 

    movaps  xmm0, [eax] ; xmm0 contiene il primo vettore a
    movaps  xmm1, [ecx] ; xmm1 contiene il secondo vettore b

    subps   xmm0, xmm1 ; a[i] = a[i] - b[i]
    mulps   xmm0, xmm0 ; a[i] = a[i] * a[i]
    haddps  xmm0, xmm0 
    haddps  xmm0, xmm0 ; a = a[0] + a[1] + a[2] + a[3]
    sqrtss  xmm0, xmm0 ; dista = sqrt(a)

    mov eax, [ebp + dista]
    movss [eax], xmm0 ; valore di ritorno per riferimento

    pop	edi		
	pop	esi
	pop	ebx
	mov	esp, ebp	
	pop	ebp		
	ret	