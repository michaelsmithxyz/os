#include <string.h>

#include "include/early_fb.h"

char *fb_mem = (char *) VIDEO_MEM_ADDR;

static char xpos;
static char ypos;

void k_fbwritecell(unsigned int cell, char ascii, FB_COLOR fg, FB_COLOR bg) {
    unsigned int real_cell = cell * 2;
    fb_mem[real_cell] = ascii;
    fb_mem[real_cell + 1] = ((bg & 0x0F) << 4) | (fg & 0x0F);
    return;
}

void k_fbputchar(char x, char y, char ascii, FB_COLOR fg, FB_COLOR bg) {
    if(x > 79 || y > 24) {
        return;
    }
    unsigned int cell = (80 * y) + x;
    k_fbwritecell(cell, ascii, fg, bg);
    return;
}

void k_fbputstring(char x, char y, char *string, FB_COLOR fg, FB_COLOR bg) {
    char c;
    while((c = *(string++)) != '\0') {
        if(c == '\n') {
            y++;
            x = 0;
        } else {
            k_fbputchar(x, y, c, fg, bg);
            x++;
        }
        if(x > 79) {
            y++;
            x = 0;
        }
        if(y > 24) {
            x = 0;
            y = 0;
        }
    }
    return;
}

void k_fbprintc(char ch) {
    if(xpos > 79) {
        xpos = 0;
        ypos++;
    }
    if(ypos > 24) {
        ypos = 0;
        xpos = 0;
    }
    if(ch == '\n') {
        xpos = 0;
        ypos++;
    } else {
        k_fbputchar(xpos, ypos, ch, FB_FG_COLOR, FB_BG_COLOR);
        xpos++;
    }
    return;
}

void k_fbprint(char *string) {
    char c;
    while((c = *(string++)) != '\0') {
        k_fbprintc(c);
    }
    return;
}

void k_fbputall(char ascii, FB_COLOR fg, FB_COLOR bg) {
    char x, y;
    for(x = 0; x < 80; x++) {
        for(y = 0; y < 25; y++) {
            k_fbputchar(x, y, ascii, fg, bg);
        }
    }
    return;
}

void k_fbclear(void) {
    k_fbputall(0, FB_WHITE, FB_BLACK);
    xpos = 0;
    ypos = 0;
    return;
}