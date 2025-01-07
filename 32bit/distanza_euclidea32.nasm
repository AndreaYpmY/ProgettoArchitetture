%include "sseutils32.nasm"

section .data

section .bss
    alignb 16
    z   resd 1

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

    ;distance euclidea tra due vettori
    ;distanza = sqrt((x1-x2)^2 + (y1-y2)^2 + (z1-z2)^2)

    mov eax, [ebp + a]
    mov ecx, [ebp + b]

    movaps  xmm0, [eax]
    movaps  xmm1, [ecx]

    subps   xmm0, xmm1
    mulps   xmm0, xmm0
    haddps  xmm0, xmm0
    haddps  xmm0, xmm0
    sqrtss  xmm0, xmm0

    mov eax, [ebp + dista]
    movss [eax], xmm0

    pop	edi		
	pop	esi
	pop	ebx
	mov	esp, ebp	
	pop	ebp		
	ret	