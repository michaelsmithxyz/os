#include <console.h>
#include <kdefs.h>
#include <memory.h>

#include <multiboot.h>


extern void *kernel_end;
uintptr_t kmalloc_ptr = NULL;


static void init_kmalloc(void) {
    kmalloc_ptr = (uintptr_t) &kernel_end;
}

static uintptr_t kmalloc_impl(size_t size, int align) {
    if(kmalloc_ptr == NULL) {
        init_kmalloc();
    }
    if(align) {
        kmalloc_ptr = kmalloc_ptr & 0xFFFFF000;
        kmalloc_ptr += 0x1000;
    }
    uintptr_t address = kmalloc_ptr;
    kmalloc_ptr += size;
    return address;
}

uintptr_t kmalloc(size_t size) {
    return kmalloc_impl(size, 0);
}

uintptr_t kamalloc(size_t size) {
    return kmalloc_impl(size, 1);
}


uint32_t *page_frame_map;
size_t page_frame_map_size;


void init_physpage_map(size_t memory) {
    size_t frames = memory * 1024 / PAGE_SIZE;
    size_t map_size = frames / (8 * 4);
    page_frame_map_size = map_size;
    page_frame_map = (uint32_t *) kmalloc(map_size);
}

#if 0
void parse_mb_map(multiboot_info_t *mbd) {
    unsigned int low_mem = mbd->mem_lower;
    unsigned int high_mem = mbd->mem_upper;

    size_t highest_page = (high_mem + 1024) / 4;
    set_max_physpage(highest_page);
    if(mbd->flags & 0x20) {
        memory_map_t *mmap = (memory_map_t *)mbd->mmap_addr;
        while((unsigned long) mmap < (mbd->mmap_addr + mbd->mmap_length)) {
            if(mmap->type != 1) {
                unsigned long end_addr = mmap->base_addr_low + mmap->length_low;
                if(end_addr == 0) {
                    end_addr = end_addr - 1;
                }
                size_t page_low = mmap-> base_addr_low >> 12;
                size_t page_high = end_addr >> 12;
                kprint("Marking pages used: ");
                kprint_hex(page_low, 8);
                kprint(" - ");
                kprint_hex(page_high, 8);
                kprint("\n");
                mark_physpage_range_used(page_low, page_high);
            }
            mmap = (memory_map_t *)((unsigned long)mmap + mmap->size + sizeof(unsigned int));
        }
    }
}
#endif
