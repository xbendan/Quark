OUTPUT_FORMAT("elf64-x86-64")
OUTPUT_ARCH(i386:x86-64)

ENTRY(kload_limine)

/* PHDRS {
    text    PT_LOAD;
    rodata  PT_LOAD;
    data    PT_LOAD;
    dynamic PT_DYNAMIC;
} */

SECTIONS {
    KERNEL_VMA = 0xffffffff80000000;

    . = 0x100000;
    .boot.text : { KEEP(*(.boot.text)) }
    .boot.data : { KEEP(*(.boot.data)) }

    . += KERNEL_VMA;

    KERNEL_START = .;
    .st2hdr ALIGN(4K) : AT(ADDR(.st2hdr) - KERNEL_VMA) {
        KEEP(*(.st2hdr))
    }

    .text ALIGN(4K) : AT(ADDR(.text) - KERNEL_VMA) {
        KEEP(*(.text*))
    }

    .rodata ALIGN(4K) : AT(ADDR(.rodata) - KERNEL_VMA) {
        KEEP(*(.rodata*))
    }

    .data ALIGN(4K) : AT(ADDR(.data) - KERNEL_VMA) {
        KEEP(*(.data*))

        /* Place the sections that contain the Limine requests as part of the .data */
        /* output section. */
        KEEP(*(.requests_start_marker))
        KEEP(*(.requests))
        KEEP(*(.requests_end_marker))
    }

    .bss ALIGN(4K) : AT(ADDR(.bss) - KERNEL_VMA) {
        KEEP(*(COMMON))
        KEEP(*(.bss*))
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