global _lgdt
_lgdt:
    lgdt [rdi]

    push rbp,
    mov rbp, rsp

    push qword 0x10
    push rbp
    pushf
    push qword 0x8
    push .trampoline
    iretq
.trampoline:
    pop rbp

    mov ax, 0x10

    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov ax, 0x28
    ltr ax

    ret

global _lidt
_lidt:
    lidt [rdi]
    ret

fpu_testword: dw 0x55AA

[bits 32]
global _lfpu
_lfpu:
    mov eax, cr0
    and eax, 0x2C
    mov cr0, eax
    fninit
    ret
    ; fnstsw [fpu_testword]
    ; cmp word[fpu_testword], 0
    ; jne .no_fpu
    ; jmp .has_fpu

[bits 64]
section .data
global __cxa_atexit
__cxa_atexit:
  ret

section .bss
global __dso_handle
__dso_handle: resq 1