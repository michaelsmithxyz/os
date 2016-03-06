[bits 32]

section .multiboot
    MAGIC_NUMBER equ 0x1BADB002
    MEMINFO      equ 1 << 1
    FLAGS        equ MEMINFO | 0x0
    CHECKSUM     equ - (MAGIC_NUMBER + FLAGS)
    
    align 0x04
    dd MAGIC_NUMBER
    dd FLAGS
    dd CHECKSUM


section .text:
    [global loader]
    [extern k_main]

    loader:
        mov esp, stack_top
        call load_gdt
        push eax
        push ebx
        call k_main

        .loop:
            jmp .loop

    global install_page_directory
    install_page_directory:
        mov eax, [esp + 4]
        mov cr3, eax
        mov ecx, cr0
        or ecx, 0x80000000
        mov cr0, eax
        ret

    load_gdt:
        push eax
        lgdt [gdt.pointer]
        mov ax, gdt.data - gdt
        mov ds, ax
        mov es, ax
        mov ss, ax
        jmp (gdt.code - gdt):load_gdt.far_jump
        
        .far_jump:
            pop eax
            ret

section .rodata
    gdt:
        dq 0
    .code:
        dd 0x0000FFFF ; Code base 0:15 + limit 0:15
        dd 0x00CF9A00
    .data:
        dd 0x0000FFFF ; Data base 0:15 + limit 0:15
        dd 0x00CF9200
    .pointer:
        dw $ - gdt - 1
        dd gdt

section .bss
    align 4
    stack_bottom:
        resb 0x4000
    stack_top:
