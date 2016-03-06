#include <console.h>
#include <memory.h>

#include <multiboot.h>


extern int kernel_start;
extern int kernel_end;


void k_main(multiboot_info_t *mbd, unsigned int magic) {
    if(magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        return;
    }
    init_early_console();
    if(!(mbd->flags & 0x01)) {
        kprint("The bootloader isn't giving us any memory information\n");
        kprint("Terminating execution!\n");
    }
    kprint("Kernel size: ");
    kprint_hex((&kernel_end - &kernel_start) * sizeof(int), 8);
    kprint(" Bytes\n");
    
    init_page_frame_map(mbd->mem_lower + mbd->mem_upper);
    if(mbd->flags & (1 << 6)) {
        kprint("== Parsing Memory Map ==\n");
        multiboot_memory_map_t *mmap = mbd->mmap_addr; 
        while((uintptr_t) mmap < mbd->mmap_addr + mbd->mmap_length) {
            kprint("Start: ");
            kprint_hex(mmap->addr, 8);
            kprint("\nLength: ");
            kprint_dec(mmap->len / 1024, 10);
            kprint("\nType: ");
            kprint_hex(mmap->type, 8);
            kprint("\n\n");
            mmap = (multiboot_memory_map_t *) ((uintptr_t) mmap + mmap->size + sizeof(uintptr_t));
        }
    }
}
