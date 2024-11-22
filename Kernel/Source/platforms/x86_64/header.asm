BITS 32

GLOBAL GDT64
GLOBAL GDT64.TSS.Low
GLOBAL GDT64.TSS.Medium
GLOBAL GDT64.TSS.High
GLOBAL GDT64.TSS.High32
GLOBAL GDT64.Pack64

extern kload_multiboot2
extern kload_stivale2

STACK_SIZE  equ 0x4000
KERNEL_VIRTUAL_BASE equ 0xFFFFFFFF80000000
KERNEL_BASE_PML4_INDEX equ (((KERNEL_VIRTUAL_BASE) >> 39) & 0x1FF)
KERNEL_BASE_PDPT_INDEX equ (((KERNEL_VIRTUAL_BASE) >> 30) & 0x1FF)
PAGE_SIZE_4K equ 0x1000

SECTION .boot.data
ALIGN PAGE_SIZE_4K
_pml4:
    times 512 dq 0

ALIGN PAGE_SIZE_4K
_pde:
    times 512 dq 0

ALIGN PAGE_SIZE_4K
_pdpt:
    dq 0 
    times 511 dq 0

ALIGN PAGE_SIZE_4K
_pdpt2:
    times KERNEL_BASE_PDPT_INDEX dq 0
    dq 0

ALIGN 0x10
GDT64:                           ; Global Descriptor Table (64-bit).
    .Null: equ $ - GDT64         ; The null descriptor.
        dw 0xFFFF                    ; Limit (low).
        dw 0                         ; Base (low).
        db 0                         ; Base (middle)
        db 0                         ; Access.
        db 0                         ; Granularity.
        db 0                         ; Base (high).
    .Code: equ $ - GDT64         ; The code descriptor.
        dw 0                         ; Limit (low).
        dw 0                         ; Base (low).
        db 0                         ; Base (middle)
        db 10011010b                 ; Access (exec/read).
        db 00100000b                 ; Granularity, 64 bits flag, limit19:16.
        db 0                         ; Base (high).
    .Data: equ $ - GDT64         ; The data descriptor.
        dw 0                         ; Limit (low).
        dw 0                         ; Base (low).
        db 0                         ; Base (middle)
        db 10010010b                 ; Access (read/write).
        db 00000000b                 ; Granularity.
        db 0                         ; Base (high).
    .UserData: equ $ - GDT64     ; The usermode data descriptor.
        dw 0                         ; Limit (low).
        dw 0                         ; Base (low).
        db 0                         ; Base (middle)
        db 11111010b                 ; Access (read/write).
        db 00100000b                 ; Granularity.
        db 0                         ; Base (high).
    .UserCode: equ $ - GDT64     ; The usermode code descriptor.
        dw 0                         ; Limit (low).
        dw 0                         ; Base (low).
        db 0                         ; Base (middle)
        db 11110010b                 ; Access (exec/read).
        db 00000000b                 ; Granularity, 64 bits flag, limit19:16.
        db 0                         ; Base (high).
    .TSS: ;equ $ - GDT64         ; TSS Descriptor
    .Length:
        dw 108                       ; TSS Length - the x86_64 TSS is 108 bytes long
    .Low:
        dw 0                         ; Base (low).
    .Medium:
        db 0                         ; Base (middle)
        db 10001001b                 ; Flags
        db 00000000b                 ; Flags 2
    .high:
        db 0                         ; Base (high).
    .High32:
        dd 0                         ; High 32 bits
        dd 0                         ; Reserved
    .Pack32:                     ; Pack
        dw $ - GDT64 - 1             ; Limit.
        dq GDT64                     ; Base.


MAGIC    equ 0xE85250D6
ARCH     equ 0
LENGTH   equ (multiboot_header_end - multiboot_header_start)
CHECKSUM equ -(MAGIC + ARCH + LENGTH)

SECTION .boot.text
align 0x08
multiboot_header_start:
    dd MAGIC
    dd ARCH
    dd LENGTH
    dd CHECKSUM

    .tag_entry:
        align 0x08
        dw 3
        dw 0
        dq 12
        dq Entry
    
    .tag_end:
        align 0x08
        dw 0
        dw 0
        dd 8
multiboot_header_end:

NotSupportedMessage db "This CPU is not supported.", 0
NotSupportedMessageEnd:

NotSupported:
    mov eax, 0xb8000
    mov edi, eax
    mov ecx, 80 * 25
    xor eax, eax
    mov ah, 0x0F
    rep stosw
    mov edi, eax
    mov esi, NotSupportedMessage
    mov ecx, NotSupportedMessageEnd - NotSupportedMessage
    rep movsb
    cli
    hlt ; Generate by Copilot, I don't know why it's here

SECTION .boot.text
GLOBAL Entry
BITS 32
Entry:
    mov dword [multiboot_address], ebx
    
    mov eax, 0x80000000
    cpuid                   ; Check if CPUID is supported
    cmp eax, 0x80000001     ; Check if extended CPUID is supported
    jb NotSupported

    mov eax, 0x80000001
    cpuid
    test edx, 1 << 29       ; Check if the CPU supports the long mode
    jz NotSupported

    mov eax, cr4
    or eax, 1 << 5          ; Enable the Physical Address Extension
    mov cr4, eax

    mov ecx, 512
    mov eax, _pde
    mov ebx, 0x83
.fill:
    mov dword [eax], ebx
    add ebx, 0x200000
    add eax, 8
    loop .fill
    
    mov eax, _pdpt
    or eax, 0x03 ; Present and writable
    mov dword [_pml4], eax

    mov eax, _pdpt2
    or eax, 0x03 ; Present and writable
    mov dword [_pml4 + KERNEL_BASE_PML4_INDEX * 8], eax

    mov eax, _pde
    or eax, 0x03 ; Present and writable
    mov dword [_pdpt], eax
    mov dword [_pdpt2 + KERNEL_BASE_PDPT_INDEX * 8], eax

    mov eax, _pml4
    mov cr3, eax

    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax

    lgdt [GDT64.Pack32]
    jmp 0x8:Entry64 - KERNEL_VIRTUAL_BASE

BITS 64
    cli
    hlt

multiboot_address:
    dd 0
    dd 0
text_address:
    dd 0
    dd 0

EXTERN _bss
EXTERN _bss_end

global GDT64Pack64

SECTION .data
GDT64Pack64:
    dw GDT64.Pack32 - GDT64 - 1
    dq GDT64 + KERNEL_VIRTUAL_BASE

SECTION .text
Entry64:
    lgdt [GDT64Pack64]

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov rdi, _bss
    mov rcx, _bss_end
    sub rcx, _bss
    xor rax, rax
    rep stosb

    mov rsp, StackTop

    mov rax, cr0
    and ax, 0xFFFB
    or ax, 0x2
    mov cr0, rax

    mov rax, cr4
    or ax, 3 << 9 ; Enable SSE instructions
    mov cr4, rax

    mov rcx, 0x277
    rdmsr
    mov rbx, 0xFFFFFFFFFFFFFF
    and rax, rbx
    mov rbx, 0x100000000000000
    or rax, rbx ; Set PA7 to Write-combining (0x1, WC)
    wrmsr

    xor rbp, rbp
    mov rdi, qword [multiboot_address]
    call kload_multiboot2

    cli
    hlt

GLOBAL kinit_stivale2
kinit_stivale2:
    cli
    cld

    lgdt [GDT64Pack64]

    mov rbx, rdi
    mov rdi, _bss
    mov rcx, _bss_end
    sub rcx, _bss
    xor rax, rax
    rep stosb

    mov rsp, StackTop

    mov rdi, rbx
    mov rbp, rsp

    push 0x10
    push rbp
    pushf
    push 0x8
    push .cont
    iretq

.cont:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov rax, cr0
    and ax, 0xFFFB		; Clear coprocessor emulation
	or ax, 0x2			; Set coprocessor monitoring
	mov cr0, rax

	;Enable SSE
	mov rax, cr4
	or ax, 3 << 9		; Set flags for SSE
	mov cr4, rax

    mov rcx, 0x277 ; PAT Model Specific Register
    rdmsr
    mov rbx, 0xFFFFFFFFFFFFFF
    and rax, rbx
    mov rbx, 0x100000000000000
    or rax, rbx  ; Set PA7 to Write-combining (0x1, WC)
    wrmsr

    xor rbp, rbp
    call kload_stivale2

    cli
    hlt

SECTION .bss
align 64
StackTop:
    resb STACK_SIZE
StackBottom: