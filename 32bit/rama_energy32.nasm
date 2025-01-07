%include "sseutils32.nasm"

section .data
    align 16
        a_phi   dd  -57.8, -57.8, -57.8, -57.8

    align 16
        a_psi   dd  -47.0, -47.0, -47.0, -47.0

    align 16
        b_phi   dd  -119.0, -119.0, -119.0, -119.0

    align 16
        b_psi   dd  113.0, 113.0, 113.0, 113.0

    align 16
        const   dd  0.5, 0.5, 0.5, 0.5


section .bss
    alignb 16
        z   resd    1

section .text


global rama

phi equ 8
psi equ 12
n   equ 16
e equ 20

rama:
    push		ebp		
	mov		ebp, esp	
	push		ebx		
	push		esi
	push		edi

    mov eax,    [ebp + phi]
    mov ebx,    [ebp + psi]
    mov edi,    [ebp + n]

    xor edx,    edx
    xorps   xmm7,   xmm7
    mov ecx,    0
for:
    movaps  xmm0,   [eax + ecx * 4]
    movaps  xmm1,   [ebx + ecx * 4]

    movaps  xmm2,   xmm0
    movaps  xmm3,   xmm1

    ;calcolo alpha_dist
    movaps  xmm4,   [a_phi]
    subps   xmm0,   xmm4
    movaps  xmm5,   [a_psi]
    subps   xmm1,   xmm5
    mulps   xmm0,   xmm0
    mulps   xmm1,   xmm1
    addps   xmm0,   xmm1
    sqrtps  xmm0,   xmm0        ;in xmm0 ho alpha_dist

    ;calcolo beta_dist
    movaps  xmm4,   [b_phi]
    subps   xmm2,   xmm4
    movaps  xmm5,   [b_psi]
    subps   xmm3,   xmm5
    mulps   xmm2,   xmm2
    mulps   xmm3,   xmm3
    addps   xmm2,   xmm3
    sqrtps  xmm2,   xmm2        ;in xmm2 ho beta_dist

    movaps  xmm4,   xmm0
    cmpltps xmm4,   xmm2        ;in xmm4 ho alpha_dist, in xmm2 ho beta_dist. Confronto se alpha_dist < beta_dist
                                ;la maschera è in xmm4

    andps   xmm0,   xmm4

    movaps  xmm4,   xmm0
    cmpnltps xmm4,  xmm2
    andps   xmm2,   xmm4

    addps   xmm0,   xmm2

    movaps  xmm5,   [const]
    mulps   xmm0,   xmm5

    haddps  xmm0,   xmm0
    haddps  xmm0,   xmm0

    addps   xmm7,   xmm0

    ; -------------------------

    movaps  xmm0,   [eax + ecx * 4+16]
    movaps  xmm1,   [ebx + ecx * 4+16]

    movaps  xmm2,   xmm0
    movaps  xmm3,   xmm1

    ;calcolo alpha_dist
    movaps  xmm4,   [a_phi]
    subps   xmm0,   xmm4
    movaps  xmm5,   [a_psi]
    subps   xmm1,   xmm5
    mulps   xmm0,   xmm0
    mulps   xmm1,   xmm1
    addps   xmm0,   xmm1
    sqrtps  xmm0,   xmm0        ;in xmm0 ho alpha_dist

    ;calcolo beta_dist
    movaps  xmm4,   [b_phi]
    subps   xmm2,   xmm4
    movaps  xmm5,   [b_psi]
    subps   xmm3,   xmm5
    mulps   xmm2,   xmm2
    mulps   xmm3,   xmm3
    addps   xmm2,   xmm3
    sqrtps  xmm2,   xmm2        ;in xmm2 ho beta_dist

    movaps  xmm4,   xmm0
    cmpltps xmm4,   xmm2        ;in xmm4 ho alpha_dist, in xmm2 ho beta_dist. Confronto se alpha_dist < beta_dist
                                ;la maschera è in xmm4

    andps   xmm0,   xmm4

    movaps  xmm4,   xmm0
    cmpnltps xmm4,  xmm2
    andps   xmm2,   xmm4

    addps   xmm0,   xmm2

    movaps  xmm5,   [const]
    mulps   xmm0,   xmm5

    haddps  xmm0,   xmm0
    haddps  xmm0,   xmm0

    addps   xmm7,   xmm0

    ; ----------------------------

    movaps  xmm0,   [eax + ecx * 4+32]
    movaps  xmm1,   [ebx + ecx * 4+32]

    movaps  xmm2,   xmm0
    movaps  xmm3,   xmm1

    ;calcolo alpha_dist
    movaps  xmm4,   [a_phi]
    subps   xmm0,   xmm4
    movaps  xmm5,   [a_psi]
    subps   xmm1,   xmm5
    mulps   xmm0,   xmm0
    mulps   xmm1,   xmm1
    addps   xmm0,   xmm1
    sqrtps  xmm0,   xmm0        ;in xmm0 ho alpha_dist

    ;calcolo beta_dist
    movaps  xmm4,   [b_phi]
    subps   xmm2,   xmm4
    movaps  xmm5,   [b_psi]
    subps   xmm3,   xmm5
    mulps   xmm2,   xmm2
    mulps   xmm3,   xmm3
    addps   xmm2,   xmm3
    sqrtps  xmm2,   xmm2        ;in xmm2 ho beta_dist

    movaps  xmm4,   xmm0
    cmpltps xmm4,   xmm2        ;in xmm4 ho alpha_dist, in xmm2 ho beta_dist. Confronto se alpha_dist < beta_dist
                                ;la maschera è in xmm4

    andps   xmm0,   xmm4

    movaps  xmm4,   xmm0
    cmpnltps xmm4,  xmm2
    andps   xmm2,   xmm4

    addps   xmm0,   xmm2

    movaps  xmm5,   [const]
    mulps   xmm0,   xmm5

    haddps  xmm0,   xmm0
    haddps  xmm0,   xmm0

    addps   xmm7,   xmm0

    ; ----------------------------

    movaps  xmm0,   [eax + ecx * 4+48]
    movaps  xmm1,   [ebx + ecx * 4+48]

    movaps  xmm2,   xmm0
    movaps  xmm3,   xmm1

    ;calcolo alpha_dist
    movaps  xmm4,   [a_phi]
    subps   xmm0,   xmm4
    movaps  xmm5,   [a_psi]
    subps   xmm1,   xmm5
    mulps   xmm0,   xmm0
    mulps   xmm1,   xmm1
    addps   xmm0,   xmm1
    sqrtps  xmm0,   xmm0        ;in xmm0 ho alpha_dist

    ;calcolo beta_dist
    movaps  xmm4,   [b_phi]
    subps   xmm2,   xmm4
    movaps  xmm5,   [b_psi]
    subps   xmm3,   xmm5
    mulps   xmm2,   xmm2
    mulps   xmm3,   xmm3
    addps   xmm2,   xmm3
    sqrtps  xmm2,   xmm2        ;in xmm2 ho beta_dist

    movaps  xmm4,   xmm0
    cmpltps xmm4,   xmm2        ;in xmm4 ho alpha_dist, in xmm2 ho beta_dist. Confronto se alpha_dist < beta_dist
                                ;la maschera è in xmm4

    andps   xmm0,   xmm4

    movaps  xmm4,   xmm0
    cmpnltps xmm4,  xmm2
    andps   xmm2,   xmm4

    addps   xmm0,   xmm2

    movaps  xmm5,   [const]
    mulps   xmm0,   xmm5

    haddps  xmm0,   xmm0
    haddps  xmm0,   xmm0

    addps   xmm7,   xmm0

    add ecx,    16
    cmp ecx,    edi
    jb   for

    mov eax, [ebp + e]
    movss [eax], xmm7

    pop	edi		
	pop	esi
	pop	ebx
	mov	esp, ebp	
	pop	ebp		
	ret	