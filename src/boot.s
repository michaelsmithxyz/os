[bits 32]

KERNEL_BASE_ADDR equ 0xC0000000
KERNEL_TABLE_OFFSET equ (KERNEL_BASE_ADDR >> 22) * 4

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
        mov esp, stack_top - KERNEL_BASE_ADDR
        ;call load_gdt
        call enable_paging
        mov esp, stack_top
        lea ecx, [high_half]
        jmp ecx
    high_half:
        push eax
        push ebx
        call k_main

        .loop:
            jmp .loop

    enable_paging:
        push eax
        push ebx
        mov eax, page_table
        sub eax, KERNEL_BASE_ADDR
        or eax, 1
        mov ebx, page_directory
        sub ebx, KERNEL_BASE_ADDR
        mov [ebx], eax
        add ebx, KERNEL_TABLE_OFFSET
        mov [ebx], eax

        ; Map the first 4 MB in our table
        mov ebx, 0x1000
        mov ecx, 0
        .map_table:
            mov eax, ecx
            mul ebx
            or eax, 0x3
            mov [(page_table - KERNEL_BASE_ADDR) + ecx * 4], eax
            inc ecx
            cmp ecx, 0x300
            jne .map_table
        mov eax, page_directory
        sub eax, KERNEL_BASE_ADDR
        mov cr3, eax
        mov eax, cr0
        or eax, 0x80000000
        mov cr0, eax
        pop ebx
        pop eax
        ret

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
    
    align 0x1000
    page_directory:
        resb 0x1000
    page_table:
        resb 0x1000
