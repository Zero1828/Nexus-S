#include <stdint.h>

// Estructura de un descriptor de segmento de la GDT (64 bits)
struct gdt_entry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t  base_middle;
    uint8_t  access;
    uint8_t  granularity;
    uint8_t  base_high;
} __attribute__((packed));

// Estructura del puntero GDT que se pasa a la instrucción LGDT
struct gdt_ptr {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

// Definición de la GDT (Null, Kernel Code, Kernel Data)
struct gdt_entry gdt_entries[3];
struct gdt_ptr   gdt_pointer;

// Función externa en ensamblador para recargar los registros de segmento y hacer el salto lejano
extern void gdt_flush(uint64_t);

void gdt_set_gate(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
    gdt_entries[num].base_low    = (base & 0xFFFF);
    gdt_entries[num].base_middle = (base >> 16) & 0xFF;
    gdt_entries[num].base_high   = (base >> 24) & 0xFF;

    gdt_entries[num].limit_low   = (limit & 0xFFFF);
    gdt_entries[num].granularity = ((limit >> 16) & 0x0F);
    gdt_entries[num].granularity |= (gran & 0xF0);
    gdt_entries[num].access      = access;
}

void gdt_init(void) {
    gdt_pointer.limit = (sizeof(struct gdt_entry) * 3) - 1;
    gdt_pointer.base  = (uint64_t)&gdt_entries;

    // 0: Descriptor nulo obligatorio
    gdt_set_gate(0, 0, 0, 0, 0);

    // 1: Descriptor de Código del Kernel (Ring 0, 64-bit plano)
    // Access: 0x9A (Presente, Ring 0, Código, Ejecutable/Leíble)
    // Granularity: 0x20 (Modo Long / 64-bit activado)
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0x20);

    // 2: Descriptor de Datos del Kernel (Ring 0, plano)
    // Access: 0x92 (Presente, Ring 0, Datos, Lectura/Escritura)
    // Granularity: 0x00
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0x00);

    // Cargar la GDT en el CPU
    gdt_flush((uint64_t)&gdt_pointer);
}