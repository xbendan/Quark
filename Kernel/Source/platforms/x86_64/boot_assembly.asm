extern kload_st2

section .bss
align 0x10
stackBottom:
    resb 0x10000
stackTop:

section .st2hdr
align 0x4
boot_st2:
    dq kload_st2
    dq stackTop
    dq 0
    dq 0

    