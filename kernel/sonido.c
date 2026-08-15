unsigned char puerto_leer_byte(unsigned short puerto);
void puerto_escribir_byte(unsigned short puerto, unsigned char dato);

void reproducir_sonido(unsigned int frecuencia) {
    if (frecuencia == 0) return;

    unsigned int divisor = 1193180 / frecuencia;

    puerto_escribir_byte(0x43, 0xB6);
    puerto_escribir_byte(0x42, (unsigned char)(divisor & 0xFF));
    puerto_escribir_byte(0x42, (unsigned char)((divisor >> 8) & 0xFF));

    unsigned char estado = puerto_leer_byte(0x61);
    if ((estado & 3) != 3) {
        puerto_escribir_byte(0x61, estado | 3);
    }
}

void silenciar_sonido() {
    unsigned char estado = puerto_leer_byte(0x61);
    puerto_escribir_byte(0x61, estado & 0xFC);
}