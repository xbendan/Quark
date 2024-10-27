OUTPUT_FORMAT("elf64-x86-64")
OUTPUT_ARCH(i386:x86-64)

ENTRY(Entry)

/* PHDRS {
    requests PT_LOAD;
    text     PT_LOAD;
    rodata   PT_LOAD;
    data     PT_LOAD;
} */

SECTIONS {
    KERNEL_VMA = 0xffffffff80000000;

    . = 0x100000; /* 1 MiB */

    .boot.text : {
        KEEP(*(.boot.text))
    }

    .boot.data : {
        KEEP(*(.boot.data))
    }

    . += KERNEL_VMA;

    KERNEL_START = .;

    .requests : {
        KEEP(*(.requests_start_marker))
        KEEP(*(.requests))
        KEEP(*(.requests_end_marker))
    }

    .stivale2hdr ALIGN(4K) : AT(ADDR(.stivale2hdr) - KERNEL_VMA) {
        KEEP(*(.stivale2hdr))
    }
	
    /* Move to the next memory page for .text */

    .text ALIGN(4K) : AT(ADDR(.text) - KERNEL_VMA) {
        KEEP(*(.text*))
    }

    /* Move to the next memory page for .rodata */

    .rodata ALIGN(4K) : AT(ADDR(.rodata) - KERNEL_VMA) {
        __ctors_start = .;
        *(SORT(.ctors*))
        __ctors_end = .;

        __dtors_start = .;
        *(SORT(.dtors*))
        __dtors_end = .;

        KEEP(*(.rodata*))
    }

    /* Move to the next memory page for .data */

    .data ALIGN(4K) : AT(ADDR(.data) - KERNEL_VMA) {
        KEEP(*(.data*))
    }

    /* NOTE: .bss needs to be the last thing mapped to :data, otherwise lots of */
    /* unnecessary zeros will be written to the binary. */
    /* If you need, for example, .init_array and .fini_array, those should be placed */
    /* above this. */
    .bss ALIGN(4K) : AT(ADDR(.bss) - KERNEL_VMA) {
        _bss = .;
        KEEP(*(COMMON))
        KEEP(*(.bss*))
        . = ALIGN(4K);
        _bss_end = .;
    }

    KERNEL_END = .;

    /DISCARD/ : {
        *(.comment)
        *(.eh_frame)
        *(.note*)
        *(.note.*)
        *(.note_*)
        *(.rel*)
        *(.rela*)
        *(.symtab)
        *(.strtab)
        *(.shstrtab)
    }
}