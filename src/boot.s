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
        mov esp, kernel_stack + KERNEL_STACK_SIZE
        call enable_paging
        call load_gdt
        push eax
        push ebx
        call k_main

        .loop:
            jmp .loop

    enable_paging:
        push eax
        push ebx
        push ecx

        mov eax, page_table
        or eax, 1
        mov [page_directory], eax
        
        ; Identity map the first 2 MiB of memory
        mov ebx, 4096
        mov ecx, 0
        .map_table:
            mov eax, ecx
            mul ebx
            or eax, 0x3
            mov [page_table + ecx * 4], eax
            inc ecx
            cmp ecx, 0x300
            jne .map_table

        mov eax, page_directory
        or eax, 3
        mov [page_directory + 1023 * 4], eax

        mov eax, page_directory
        mov cr3, eax
        mov eax, cr0
        or eax, 0x80000000
        mov cr0, eax
        
        pop ecx
        pop ebx
        pop eax
        ret

    load_gdt:
        push eax
        lgdt [gdt.pointer]
        mov ax, gdt.data - gdt
        mov ss, ax
        mov ds, ax
        mov es, ax
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
    KERNEL_STACK_SIZE equ 4096
    
    alignb 4096
    page_directory:
        resb 4096
    
    alignb 4096
    page_table:
        resb 4096
    
    alignb 4
    kernel_stack:
        resb KERNEL_STACK_SIZE 
