#include <console.h>
#include <memory.h>

#include <multiboot.h>


extern uint8_t kernel_start;
extern uint8_t kernel_end;
extern uintptr_t kmalloc_ptr;


void k_main(multiboot_info_t *mbd, unsigned int magic) {
    if(magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        return;
    }
    init_early_console();
#if 0
    if(!(mbd->flags & 0x01)) {
        kprint("The bootloader isn't giving us any memory information\n");
        kprint("Terminating execution!\n");
    }
#endif
    size_t kernel_size = &kernel_end - &kernel_start;
    kprint("Loaded at: ");
    kprint_hex(&kernel_start, 8);
    kprint("\n");
    kprint("Kernel size: ");
    kprint_hex(kernel_size, 8);
    kprint(" Bytes\n");
#if 0
    init_page_frame_map(mbd->mem_lower + mbd->mem_upper);
    if(mbd->flags & (1 << 6)) {
        multiboot_memory_map_t *mmap = (multiboot_memory_map_t *) mbd->mmap_addr; 
        while((uintptr_t) mmap < mbd->mmap_addr + mbd->mmap_length) {
            if(mmap->type != 1) {
                for(uint64_t i = 0; i < mmap->len; i += PAGE_SIZE) {
                    if(mmap->addr + i > 0xFFFFFFFF) {
                        break;
                    }
                    mark_frame((mmap->addr + i) & 0xFFFFF000);
                }
            }
            mmap = (multiboot_memory_map_t *) ((uintptr_t) mmap + mmap->size + sizeof(uintptr_t));
        }
    }
    init_paging();
    // Let's just not allocate low memory
    for(uintptr_t addr = 0; addr < kmalloc_ptr; addr += PAGE_SIZE) {
        mark_frame(addr);
    }
#endif
}
