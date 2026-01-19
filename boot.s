# Multiboot header
.set MAGIC,    0x1BADB002
.set FLAGS,    0x03
.set CHECKSUM, -(MAGIC + FLAGS)

.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

.section .text
.global _start

_start:
    # Set up 32-bit stack
    mov $stack_top, %esp
    
    # Call the C code
    call kmain

halt:
    hlt
    jmp halt

.section .bss
.align 16
stack_bottom:
.skip 16384
stack_top: