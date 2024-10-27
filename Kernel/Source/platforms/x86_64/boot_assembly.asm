extern kinit_stivale2

section .bss
align 0x10
stackBottom:
    resb 0x10000
stackTop:

section .stivale2hdr
align 0x4
boot_st2:
    dq kinit_stivale2
    dq 0
    dq 0
    dq 0