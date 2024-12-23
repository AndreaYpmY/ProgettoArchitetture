%include "sseutils32.nasm"

section.data
    a   equ     8
    b   equ     12
    n   equ     16


section.bss


section.text


global prodotto_scalare


prodotto_scalare:
    push ebp
    mov ebp, esp
    push ebx
    push esi
    push edi


    movaps  xmm0, [ebp + a]
    movaps  xmm1, [ebp + b]
    mulps   xmm0, xmm1
    haddps  xmm0, xmm0
    haddps  xmm0, xmm0
    movss   eax, xmmo0


    pop edi
    pop esi
    pop ebx
    mov esp, ebp
    pop ebp
    ret