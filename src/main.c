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
    
    init_physpage_map(mbd->mem_lower + mbd->mem_upper);
    //parse_mb_map(mbd);
}
