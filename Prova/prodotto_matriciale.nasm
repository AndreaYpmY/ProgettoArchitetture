%include "sseutils32.nasm"

section .data

section .bss
    alignb 16
    z   resd 1

section .text


global prodmat

a equ 8
b equ 12

prodmat:
    start

    mov eax, [ebp + a]
    mov ecx, [ebp + b]
    ; continua    
    

    stop