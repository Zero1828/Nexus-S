#ifndef PANTALLA_H
#define PANTALLA_H

// Declaramos las funciones para que kernel.c las conozca
void limpiar_pantalla(void);
void kprintf(const char* format, ...);
void kputc(char caracter);

#endif // PANTALLA_H