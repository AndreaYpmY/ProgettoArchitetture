%include "sseutils32.nasm"

section .data

section .bss
    alignb 16
    z   resd 1

section .text


global prodscal

a equ 8
b equ 12

prodscal:
    push		ebp		
	mov		ebp, esp	
	push		ebx		
	push		esi
	push		edi

    mov eax, [ebp + a]
    mov ecx, [ebp + b]
        
    movaps  xmm0, [eax]
    movaps  xmm1, [ecx]

    mulps   xmm0, xmm1
    haddps  xmm0, xmm0
    haddps  xmm0, xmm0
    
    ;shufps  xmm0, xmm0, 0
    ;divps   xmm1, xmm0
    ;movaps  [z],  xmm1
    ;lea     eax,  [z]

    movss   [z], xmm0
    fld dword [z]

    pop	edi		
	pop	esi
	pop	ebx
	mov	esp, ebp	
	pop	ebp		
	ret	