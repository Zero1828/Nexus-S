#ifndef GDT_H
#define GDT_H

// Estructura de una entrada en la GDT (8 bytes)
struct gdt_entry {
    unsigned short limit_low;
    unsigned short base_low;
    unsigned char  base_middle;
    unsigned char  access;
    unsigned char  granularity;
    unsigned char  base_high;
} __attribute__((packed));

// Estructura del registro GDTR (para la instrucción lgdt)
struct gdt_ptr {
    unsigned short limit;
    unsigned int   base;
} __attribute__((packed));

void gdt_init(void);

#endif