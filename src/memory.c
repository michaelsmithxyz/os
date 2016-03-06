#include <console.h>
#include <kdefs.h>
#include <memory.h>
#include <string.h>

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


void free_frame(uintptr_t frame_ptr) {
    size_t frame = frame_ptr / 0x1000;
    size_t index = frame / (8 * 4);
    if(index < (page_frame_map_size * 4)) {
        size_t offset = frame % (8 * 4);
        page_frame_map[index] = page_frame_map[index] & ~(1 << offset);
    }
}

void mark_frame(uintptr_t frame_ptr) {
    size_t frame = frame_ptr / 0x1000;
    size_t index = frame / (8 * 4);
    if(index < (page_frame_map_size * 4)) {
        size_t offset = frame % (8 * 4);
        page_frame_map[index] = page_frame_map[index] | (1 << offset);
    }
}

// Returns 1 if the frame is used
int check_frame(uintptr_t frame_ptr) {
    size_t frame = frame_ptr / 0x1000;
    size_t index = frame / (8 * 4);
    if(index < (page_frame_map_size * 4)) {
        size_t offset = frame % (8 * 4);
        return page_frame_map[index] & (1 << offset);
    }
    return 1;
}

uintptr_t get_frame(void) {
    uintptr_t frame_addr = NULL;
    for(size_t index = 0; index < (page_frame_map_size * 4); index++) {
        if(page_frame_map[index] != 0xFFFFFFFF) {
            uint32_t bitmap = page_frame_map[index];
            for(size_t offset = 0; offset < 32; offset++) {
                if(!((bitmap >> offset) & 0x1)) {
                   size_t frameno = index * (8 * 4) + offset;
                   frame_addr = frameno * 0x1000;
                   mark_frame(frame_addr);
                   return frame_addr;
                }
            }
        }
    }
    return frame_addr;
}

void init_page_frame_map(size_t memory) {
    size_t frames = memory * 1024 / PAGE_SIZE;
    kprint("Total Frames: ");
    kprint_dec(frames, 20);
    kprint("\n");
    size_t map_size = frames / (8 * 4);
    page_frame_map_size = map_size;
    page_frame_map = (uint32_t *) kmalloc(map_size);
    memset(page_frame_map, 0, page_frame_map_size);
}


page_dir_entry_t *k_page_directory;


static void make_page_table_entry(page_table_entry_t *entry, uintptr_t address) {
    entry->address = address / PAGE_SIZE;
    entry->g = 0;
    entry->page_size = 0;
    entry->cache_disable = 0;
    entry->write_through = 0;
    entry->user = 0;
    entry->read_write = 1;
    entry->present = 1;
    entry->flags = 0;
    entry->accessed = 0;
    entry->dirty = 0;
}

void init_paging(void) {
    k_page_directory = (page_dir_entry_t *) kamalloc(sizeof(page_dir_entry_t) * 1024);
    page_table_entry_t *table = (page_table_entry_t *) kamalloc(sizeof(page_table_entry_t) * 1024);
    // Identity map the first 3 MB
    for(size_t i = 0; i < 0x300; i++) {
        make_page_table_entry(&(table[i]), i * PAGE_SIZE);
    }
    make_page_table_entry(&k_page_directory[0], (uintptr_t) table);
    make_page_table_entry(&k_page_directory[1023], (uintptr_t) k_page_directory);
    uintptr_t ret = (uintptr_t) install_page_directory((uintptr_t) k_page_directory);
    if(ret != (uintptr_t) k_page_directory) {
        kprint("Problem installing page directory\n");
        return;
    }
    kprint("Paging enabled\n");
}
