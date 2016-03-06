#ifndef _K_MEMORY_H_
#define _K_MEMORY_H_

#include <kdefs.h>

#include <multiboot.h>

#define PAGE_SIZE 4096

uintptr_t kmalloc(size_t size);
uintptr_t kamalloc(size_t size);

void init_page_frame_map(size_t memory);

#endif
