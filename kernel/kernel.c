/*
 * ============================================================================
 * Proyecto: Nexus-S Kernel
 * Descripción: Kernel principal conectado a pantalla, teclado y sonido
 * ============================================================================
 */

// 1. Incluimos las cabeceras para conectar los nuevos módulos
#include "../include/pantalla.h"
#include "../include/kernel.h"
#include "../include/teclado.h"
#include "../include/sonido.h"

#define VGA_ADDRESS 0xB8000

// Mantener tus funciones de color originales para no perder tu progreso
unsigned char vga_entry_color(unsigned char fg, unsigned char bg) {
    return fg | bg << 4;
}

unsigned short vga_entry(unsigned char uc, unsigned char color) {
    return (unsigned short) uc | (unsigned short) color << 8;
}

// Función principal del Kernel a donde saltará el bootloader
void kmain(void) {
    // 1. Inicializar la pantalla con tu controlador avanzado
    limpiar_pantalla();
    
    // 2. Dar un pitido alegre (Nota LA - 440Hz) para avisar que el hardware encendió
    reproducir_sonido(2500);
    for(volatile int i = 0; i < 5000000; i++); // Un retraso corto para escuchar el bip
    silenciar_sonido(); // Apagar el altavoz
    
    // 3. Pintar la interfaz de bienvenida oficial de Nexus-S
    kprintf("====================================================================\n");
    kprintf("                     === BIENVENIDO A NEXUS-S OS ===                \n");
    kprintf("                  [KERNEL CARGADO CON EXITO EN 32-BITS]             \n");
    kprintf("====================================================================\n\n");
    kprintf("Sistema operativo iniciado correctamente.\n");
    kprintf("Escribe 'help' para ver los comandos disponibles.\n\n");
    kprintf("Nexus-S> ");

    // 4. Bucle infinito para mantener el sistema vivo y procesando el teclado
    while (1) {
        // Llama constantemente al controlador avanzado con soporte para 'ñ'
        procesar_teclado();
        
        // Retraso de hardware para no sobrecargar el procesador emulado en QEMU
        for(volatile int i = 0; i < 50000; i++); 
    }
}