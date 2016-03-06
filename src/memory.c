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


void mark_frame(uintptr_t frame_ptr) {
    size_t frame = frame_ptr / 0x1000;
    size_t index = frame / (8 * 4);
    if(index < (page_frame_map_size * 4)) {
        size_t offset = frame % (8 * 4);
        page_frame_map[index] = page_frame_map[index] | (1 << offset);
    }
}

void init_page_frame_map(size_t memory) {
    size_t frames = memory * 1024 / PAGE_SIZE;
    kprint("Total Frames: ");
    kprint_dec(frames, 20);
    kprint("\n");
    size_t map_size = frames / (8 * 4);
    page_frame_map_size = map_size;
    page_frame_map = (uint32_t *) kmalloc(map_size);
}
