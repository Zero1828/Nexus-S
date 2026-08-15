// ============================================================================
// Proyecto: Nexus-S Kernel - Controlador de Teclado Avanzado
// ============================================================================

#include "../include/pantalla.h"

#define PUERTO_DATA_TECLADO 0x60
#define TAMANO_BUFFER 256

// Variables globales para almacenar lo que el usuario escribe
static char buffer_comando[TAMANO_BUFFER];
static int buffer_indice = 0;

// Declaramos la función del puerto que está en pantalla.c
unsigned char puerto_leer_byte(unsigned short puerto);

// Mapa de caracteres (Scancodes Set 1) - Teclado Estándar
static const char mapa_teclado[] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
  '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0,  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',   0,
  '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',   0, '*',   0, ' '
};

// Función interna para procesar los comandos cuando el usuario presiona Enter
void ejecutar_comando(const char* comando) {
    kprintf("\n"); // Salto de línea antes de responder
    
    // Compara comandos de forma manual y ligera sin usar string.h externo
    if (comando[0] == 'h' && comando[1] == 'e' && comando[2] == 'l' && comando[3] == 'p') {
        kprintf("Comandos de Nexus-S disponibles:\n");
        kprintf("  help   - Muestra este menu de ayuda\n");
        kprintf("  clear  - Limpia la pantalla por completo\n");
        kprintf("  about  - Muestra informacion del creador\n");
    } 
    else if (comando[0] == 'c' && comando[1] == 'l' && comando[2] == 'e' && comando[3] == 'a' && comando[4] == 'r') {
        limpiar_pantalla();
    } 
    else if (comando[0] == 'a' && comando[1] == 'b' && comando[2] == 'o' && comando[3] == 'u' && comando[4] == 't') {
        kprintf("Nexus-S OS v0.1\n");
        kprintf("Creado por un ingeniero de 11 anos inspirado en Linus Torvalds.\n");
    } 
    else if (comando[0] != '\0') {
        kprintf("Error: Comando no reconocido. Escribe 'help'.\n");
    }
    
    // Volver a pintar el indicador de la terminal
    kprintf("\nNexus-S> ");
}

// Función principal que lee el puerto del teclado
void procesar_teclado() {
    unsigned char scancode = puerto_leer_byte(PUERTO_DATA_TECLADO);

    // Si el bit 7 está activo, la tecla fue soltada (ignorar por ahora)
    if (scancode & 0x80) {
        return;
    }

    if (scancode < sizeof(mapa_teclado)) {
        char caracter = mapa_teclado[scancode];

        if (caracter != 0) {
            // Manejo de la tecla Enter
            if (caracter == '\n') {
                buffer_comando[buffer_indice] = '\0'; // Cerrar la cadena de texto
                ejecutar_comando(buffer_comando);     // Procesar el comando
                buffer_indice = 0;                    // Resetear el búfer
            } 
            // Manejo de Borrar (Backspace)
            else if (caracter == '\b') {
                if (buffer_indice > 0) {
                    buffer_indice--;
                    buffer_comando[buffer_indice] = '\0';
                    kputc('\b'); // Borrar visualmente en la pantalla
                }
            } 
            // Caracteres normales (A-Z, números, espacio)
            else {
                if (buffer_indice < TAMANO_BUFFER - 1) {
                    buffer_comando[buffer_indice] = caracter;
                    buffer_indice++;
                    kputc(caracter); // Pintar en pantalla
                }
            }
        }
    }
}