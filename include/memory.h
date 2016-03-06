#ifndef _K_MEMORY_H_
#define _K_MEMORY_H_

#include <kdefs.h>

#include <multiboot.h>

#define PAGE_SIZE 4096

struct page_directory_entry {
    uint32_t present: 1;
    uint32_t read_write: 1;
    uint32_t user: 1;
    uint32_t write_through: 1;
    uint32_t cache_disable: 1;
    uint32_t accessed: 1;
    uint32_t dirty: 1;
    uint32_t page_size: 1;
    uint32_t g: 1;
    uint32_t flags: 3;
    uint32_t address: 20;

} __attribute__((packed));
typedef struct page_directory_entry page_dir_entry_t;
typedef struct page_directory_entry page_table_entry_t;


uintptr_t kmalloc(size_t size);
uintptr_t kamalloc(size_t size);

void free_frame(uintptr_t frame_ptr);
void mark_frame(uintptr_t frame_ptr);
int check_frame(uintptr_t frame_ptr);
uintptr_t get_frame(void);

void init_page_frame_map(size_t memory);

void init_paging(void);

uintptr_t install_page_directory(uintptr_t page_directory);

#endif
