#include <kdefs.h>
#include <string.h>

size_t strlen(char *string) {
    unsigned int len;
    for(len = 0; *(string++) != '\0'; len++);
    return len;
}

void *memset(void *b, uint8_t c, size_t size) {
    uint8_t *p = b;
    while(size--) {
        *p++ = c;
    }
    return b;
}
