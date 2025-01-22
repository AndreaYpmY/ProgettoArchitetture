%include "sseutils64.nasm"

section .data
    align 32
        a_phi   dq  -57.8, -57.8, -57.8, -57.8

    align 32
        a_psi   dq  -47.0, -47.0, -47.0, -47.0

    align 32
        b_phi   dq  -119.0, -119.0, -119.0, -119.0

    align 32
        b_psi   dq  113.0, 113.0, 113.0, 113.0

    align 32
        const   dq  0.5, 0.5, 0.5, 0.5


section .bss


section .text


global rama


rama:
    push    rbp				; salva il Base Pointer
	mov		rbp, rsp		; il Base Pointer punta al Record di Attivazione corrente
	pushaq


    xorps   xmm7,   xmm7
    mov rbx,    0
for:
    vmovapd ymm0,   [rdi + rbx * 8]
    vmovapd ymm1,   [rsi + rbx * 8]

    vmovapd ymm2,   ymm0
    vmovapd ymm3,   ymm1

    ;calcolo alpha_dist
    vmovapd ymm4,   [a_phi]
    vsubpd  ymm0,   ymm4
    vmovapd ymm5,   [a_psi]
    vsubpd  ymm1,   ymm5
    vmulpd  ymm0,   ymm0
    vmulpd  ymm1,   ymm1
    vaddpd  ymm0,   ymm1
    vsqrtpd ymm0,   ymm0        ;in ymm0 ho alpha_dist

    ;calcolo beta_dist
    vmovapd ymm4,   [b_phi]
    vsubpd  ymm2,   ymm4
    vmovapd ymm5,   [b_psi]
    vsubpd  ymm3,   ymm5
    vmulpd  ymm2,   ymm2
    vmulpd  ymm3,   ymm3
    vaddpd  ymm2,   ymm3
    vsqrtpd ymm2,   ymm2        ;in ymm2 ho beta_dist


    vmovapd  ymm4,   ymm0
    vcmpltpd    ymm4,   ymm2        ;in xmm4 ho alpha_dist, in xmm2 ho beta_dist. Confronto se alpha_dist < beta_dist
                                ;la maschera è in xmm4

    vandpd   ymm0,   ymm4

    vmovapd  ymm4,   ymm0
    vcmpnltpd   ymm4,  ymm2
    vandpd   ymm2,   ymm4

    vaddpd   ymm0,   ymm2

    vmovapd  ymm5,   [const]
    vmulpd   ymm0,   ymm5

    vhaddpd  ymm0,   ymm0
    vperm2f128  ymm1,   ymm0,   ymm0,   00000001b
    addsd   xmm0, xmm1

    addpd   xmm7,   xmm0


    ;-----------------------------------------------------------

    vmovapd ymm0,   [rdi + rbx * 8 + 32]
    vmovapd ymm1,   [rsi + rbx * 8 + 32]

    vmovapd ymm2,   ymm0
    vmovapd ymm3,   ymm1

    ;calcolo alpha_dist
    vmovapd ymm4,   [a_phi]
    vsubpd  ymm0,   ymm4
    vmovapd ymm5,   [a_psi]
    vsubpd  ymm1,   ymm5
    vmulpd  ymm0,   ymm0
    vmulpd  ymm1,   ymm1
    vaddpd  ymm0,   ymm1
    vsqrtpd ymm0,   ymm0        ;in ymm0 ho alpha_dist

    ;calcolo beta_dist
    vmovapd ymm4,   [b_phi]
    vsubpd  ymm2,   ymm4
    vmovapd ymm5,   [b_psi]
    vsubpd  ymm3,   ymm5
    vmulpd  ymm2,   ymm2
    vmulpd  ymm3,   ymm3
    vaddpd  ymm2,   ymm3
    vsqrtpd ymm2,   ymm2        ;in ymm2 ho beta_dist


    vmovapd  ymm4,   ymm0
    vcmpltpd    ymm4,   ymm2        ;in xmm4 ho alpha_dist, in xmm2 ho beta_dist. Confronto se alpha_dist < beta_dist
                                ;la maschera è in xmm4

    vandpd   ymm0,   ymm4

    vmovapd  ymm4,   ymm0
    vcmpnltpd   ymm4,  ymm2
    vandpd   ymm2,   ymm4

    vaddpd   ymm0,   ymm2

    vmovapd  ymm5,   [const]
    vmulpd   ymm0,   ymm5

    vhaddpd  ymm0,   ymm0
    vperm2f128  ymm1,   ymm0,   ymm0,   00000001b
    addsd   xmm0, xmm1

    addpd   xmm7,   xmm0


    ;----------------------------------------------------

    vmovapd ymm0,   [rdi + rbx * 8 + 64]
    vmovapd ymm1,   [rsi + rbx * 8 + 64]

    vmovapd ymm2,   ymm0
    vmovapd ymm3,   ymm1

    ;calcolo alpha_dist
    vmovapd ymm4,   [a_phi]
    vsubpd  ymm0,   ymm4
    vmovapd ymm5,   [a_psi]
    vsubpd  ymm1,   ymm5
    vmulpd  ymm0,   ymm0
    vmulpd  ymm1,   ymm1
    vaddpd  ymm0,   ymm1
    vsqrtpd ymm0,   ymm0        ;in ymm0 ho alpha_dist

    ;calcolo beta_dist
    vmovapd ymm4,   [b_phi]
    vsubpd  ymm2,   ymm4
    vmovapd ymm5,   [b_psi]
    vsubpd  ymm3,   ymm5
    vmulpd  ymm2,   ymm2
    vmulpd  ymm3,   ymm3
    vaddpd  ymm2,   ymm3
    vsqrtpd ymm2,   ymm2        ;in ymm2 ho beta_dist


    vmovapd  ymm4,   ymm0
    vcmpltpd    ymm4,   ymm2        ;in xmm4 ho alpha_dist, in xmm2 ho beta_dist. Confronto se alpha_dist < beta_dist
                                ;la maschera è in xmm4

    vandpd   ymm0,   ymm4

    vmovapd  ymm4,   ymm0
    vcmpnltpd   ymm4,  ymm2
    vandpd   ymm2,   ymm4

    vaddpd   ymm0,   ymm2

    vmovapd  ymm5,   [const]
    vmulpd   ymm0,   ymm5

    vhaddpd  ymm0,   ymm0
    vperm2f128  ymm1,   ymm0,   ymm0,   00000001b
    addsd   xmm0, xmm1

    addpd   xmm7,   xmm0

    ;---------------------------------------------------------------

    vmovapd ymm0,   [rdi + rbx * 8 + 96]
    vmovapd ymm1,   [rsi + rbx * 8 + 96]

    vmovapd ymm2,   ymm0
    vmovapd ymm3,   ymm1

    ;calcolo alpha_dist
    vmovapd ymm4,   [a_phi]
    vsubpd  ymm0,   ymm4
    vmovapd ymm5,   [a_psi]
    vsubpd  ymm1,   ymm5
    vmulpd  ymm0,   ymm0
    vmulpd  ymm1,   ymm1
    vaddpd  ymm0,   ymm1
    vsqrtpd ymm0,   ymm0        ;in ymm0 ho alpha_dist

    ;calcolo beta_dist
    vmovapd ymm4,   [b_phi]
    vsubpd  ymm2,   ymm4
    vmovapd ymm5,   [b_psi]
    vsubpd  ymm3,   ymm5
    vmulpd  ymm2,   ymm2
    vmulpd  ymm3,   ymm3
    vaddpd  ymm2,   ymm3
    vsqrtpd ymm2,   ymm2        ;in ymm2 ho beta_dist


    vmovapd  ymm4,   ymm0
    vcmpltpd    ymm4,   ymm2        ;in xmm4 ho alpha_dist, in xmm2 ho beta_dist. Confronto se alpha_dist < beta_dist
                                ;la maschera è in xmm4

    vandpd   ymm0,   ymm4

    vmovapd  ymm4,   ymm0
    vcmpnltpd   ymm4,  ymm2
    vandpd   ymm2,   ymm4

    vaddpd   ymm0,   ymm2

    vmovapd  ymm5,   [const]
    vmulpd   ymm0,   ymm5

    vhaddpd  ymm0,   ymm0
    vperm2f128  ymm1,   ymm0,   ymm0,   00000001b
    addsd   xmm0, xmm1

    addpd   xmm7,   xmm0

    add rbx,    16
    cmp rbx,    rdx
    jb   for


    vmovsd [rcx], xmm7          ; valore di ritorno per riferimento

    popaq				; ripristina i registri generali
	mov		rsp, rbp	; ripristina lo Stack Pointer
	pop		rbp		    ; ripristina il Base Pointer
	ret