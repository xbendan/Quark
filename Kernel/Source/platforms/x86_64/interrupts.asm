%macro pushaq 0
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
%endmacro

%macro popaq 0
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
%endmacro

intNorm:
    cld
    pushaq
    mov rdi, rsp
    call dispatchInterrupts
    mov rsp, rax
    popaq
    add rsp, 16
    iretq

%macro _intErr_ 1

_int%1:
    push %1
    jmp intNorm

%endmacro

%macro _intNoErr_ 1

_int%1:
    push 0
    push %1
    jmp intNorm

%endmacro


section .text
extern dispatchInterrupts

_intNoErr_ 0
_intNoErr_ 1
_intNoErr_ 2
_intNoErr_ 3
_intNoErr_ 4
_intNoErr_ 5
_intNoErr_ 6
_intNoErr_ 7
_intErr_ 8
_intNoErr_ 9
_intErr_ 10
_intErr_ 11
_intErr_ 12
_intErr_ 13
_intErr_ 14
_intNoErr_ 15
_intNoErr_ 16
_intNoErr_ 17
_intNoErr_ 18
_intNoErr_ 19
_intNoErr_ 20
_intNoErr_ 21
_intNoErr_ 22
_intNoErr_ 23
_intNoErr_ 24
_intNoErr_ 25
_intNoErr_ 26
_intNoErr_ 27
_intNoErr_ 28
_intNoErr_ 29
_intErr_ 30
_intNoErr_ 31

%assign i 32
%rep 224
_intNoErr_ i
%assign i i+1
%endrep

section .data
global interruptVectors

interruptVectors:
%assign i 0
%rep 256
    dq _int %+ i
%assign i i+1
%endrep