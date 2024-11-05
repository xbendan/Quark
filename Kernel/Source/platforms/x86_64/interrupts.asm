extern isr_handler
extern irq_handler
extern ipi_handler

section .text
bits 64

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

%macro ISR_ERROR_CODE 1
	global _int%1
	_int%1:
        cli
        pushaq
        mov rdi, %1
        mov rsi, rsp
        xor rbp, rbp
        call isr_handler
        popaq
        add rsp, 8; Remove the error code
        iretq
%endmacro

%macro ISR_NO_ERROR_CODE 1
	global _int%1
	_int%1:
		cli
        push 0
        pushaq
        mov rdi, %1
        mov rsi, rsp
        xor rbp, rbp
        call isr_handler
        popaq
        add rsp, 8; Remove the error code
        iretq
%endmacro

%macro IRQ 2
    global _int%2
    _int%2:
        cli
        push 0
        pushaq
        mov rdi, %2
        mov rsi, rsp
        xor rdx, rdx
        xor rbp, rbp
        call irq_handler
        popaq
        add rsp, 8; Remove the error code
        iretq
%endmacro

%macro IPI 1
	global _int%1
	_int%1:
		cli
        push 0
        pushaq
        mov rdi, %1
        mov rsi, rsp
        xor rbp, rbp
        call ipi_handler
        popaq
        add rsp, 8; Remove the error code
        iretq
%endmacro

; intNorm:
;     cld
;     pushaq
;     mov rdi, rsp
;     call dispatchInterrupts
;     mov rsp, rax
;     popaq
;     add rsp, 16
;     iretq

; %macro _intErr_ 1

; _int%1:
;     push %1
;     jmp intNorm

; %endmacro

; %macro _intNoErr_ 1

; _int%1:
;     push 0
;     push %1
;     jmp intNorm

; %endmacro


ISR_NO_ERROR_CODE  0
ISR_NO_ERROR_CODE  1
ISR_NO_ERROR_CODE  2
ISR_NO_ERROR_CODE  3
ISR_NO_ERROR_CODE  4
ISR_NO_ERROR_CODE  5
ISR_NO_ERROR_CODE  6
ISR_NO_ERROR_CODE  7
ISR_ERROR_CODE 8
ISR_NO_ERROR_CODE  9
ISR_ERROR_CODE 10
ISR_ERROR_CODE 11
ISR_ERROR_CODE 12
ISR_ERROR_CODE 13
ISR_ERROR_CODE 14
ISR_NO_ERROR_CODE  15
ISR_NO_ERROR_CODE  16
ISR_ERROR_CODE  17
ISR_NO_ERROR_CODE  18
ISR_NO_ERROR_CODE 19
ISR_NO_ERROR_CODE 20
ISR_NO_ERROR_CODE 21
ISR_NO_ERROR_CODE 22
ISR_NO_ERROR_CODE 23
ISR_NO_ERROR_CODE 24
ISR_NO_ERROR_CODE 25
ISR_NO_ERROR_CODE 26
ISR_NO_ERROR_CODE 27
ISR_NO_ERROR_CODE 28
ISR_NO_ERROR_CODE 29
ISR_ERROR_CODE 30
ISR_NO_ERROR_CODE 31
; ISR_NO_ERROR_CODE 32

%assign num 48
%rep 256-48
    IPI num
%assign num (num + 1)
%endrep

%assign num 0
%rep 16
%assign i (num + 32)
    IRQ num, i
%assign num (num + 1)
%endrep

; _intNoErr_ 0
; _intNoErr_ 1
; _intNoErr_ 2
; _intNoErr_ 3
; _intNoErr_ 4
; _intNoErr_ 5
; _intNoErr_ 6
; _intNoErr_ 7
; _intErr_ 8
; _intNoErr_ 9
; _intErr_ 10
; _intErr_ 11
; _intErr_ 12
; _intErr_ 13
; _intErr_ 14
; _intNoErr_ 15
; _intNoErr_ 16
; _intNoErr_ 17
; _intNoErr_ 18
; _intNoErr_ 19
; _intNoErr_ 20
; _intNoErr_ 21
; _intNoErr_ 22
; _intNoErr_ 23
; _intNoErr_ 24
; _intNoErr_ 25
; _intNoErr_ 26
; _intNoErr_ 27
; _intNoErr_ 28
; _intNoErr_ 29
; _intErr_ 30
; _intNoErr_ 31

; %assign i 32
; %rep 224
; _intNoErr_ i
; %assign i i+1
; %endrep

section .rodata
global IntVec

IntVec:
%assign i 0
%rep 256
    dq _int %+ i
%assign i i+1
%endrep