#include <console.h>
#include <io.h>
#include <kdefs.h>

struct console_struct system_console = { .writec = NULL, .readc = NULL};

/*
 * These routines get the console set up as quickly and cheaply as possible over serial.
 * They are meant to be used early in the boot process before any real serial driver
 * is loaded.
 */

#define COM1 0x3F8

static void init_serial(uint16_t address) {
    outb(address + 1, 0x00); // Disable interrupts
    outb(address + 3, 0x80); // 0x80 == 10000000 (enable DLAB)
    outb(address + 0, 0x03); // Set baud rate divisor to three (low byte)
    outb(address + 1, 0x00); // " (high byte)
    outb(address + 3, 0x03); // Character length = 8 bits
    outb(address + 2, 0xC7);
    outb(address + 4, 0x0B);
}

static void write_serial(uint16_t address, char ascii) {
    outb(address, ascii);
}

static void early_serial_write(char ascii) {
    write_serial(COM1, ascii);
}

void init_early_console(void) {
    init_serial(COM1);
    system_console.writec = early_serial_write;
    kprint("Initialized early console\n");
}


void kprint(char *string) {
    while(*string != '\0') {
        system_console.writec(*string);
        string++;
    }
}

void kprint_hex(unsigned int value, int width) {
    char buf[width + 3];
    int index = width + 1;
    buf[width + 2] = '\0';
    while(value > 0) {
        buf[index] = "0123456789ABCDEF"[value % 0x10];
        value = value / 0x10;
        index--;
    }
    while(index >= 0) {
        buf[index--] = '0';
    }
    buf[0] = '0';
    buf[1] = 'x';
    kprint(buf);
}

void kprint_dec(unsigned int value, int width) {
    char buf[width + 1];
    int index = width - 1;
    buf[width] = '\0';
    while(value > 0) {
        buf[index] = "0123456789"[value % 10];
        value = value / 10;
        index--;
    }
    kprint(buf + index + 1);
}
