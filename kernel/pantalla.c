#define REG_SCREEN_CTRL 0x3D4
#define REG_SCREEN_DATA 0x3D5

static int cursor_fila = 0;
static int cursor_columna = 0;
static unsigned char color_texto_actual = 0x0F;

unsigned char puerto_leer_byte(unsigned short puerto) {
    unsigned char resultado;
    __asm__ volatile("inb %1, %0" : "=a"(resultado) : "Nd"(puerto));
    return resultado;
}

void puerto_escribir_byte(unsigned short puerto, unsigned char dato) {
    __asm__ volatile("outb %0, %1" :: "a"(dato), "Nd"(puerto));
}

void actualizar_cursor() {
    unsigned short posicion = (cursor_fila * 80) + cursor_columna;
    puerto_escribir_byte(REG_SCREEN_CTRL, 14);
    puerto_escribir_byte(REG_SCREEN_DATA, (unsigned char)(posicion >> 8));
    puerto_escribir_byte(REG_SCREEN_CTRL, 15);
    puerto_escribir_byte(REG_SCREEN_DATA, (unsigned char)(posicion & 0xFF));
}

void hacer_scroll() {
    if (cursor_fila >= 25) {
        char* memoria_video = (char*)0xB8000;
        for (int i = 1; i < 25; i++) {
            for (int j = 0; j < 80 * 25; j++) {
                memoria_video[((i - 1) * 160) + j] = memoria_video[(i * 160) + j];
            }
        }
        char* ultima_fila = memoria_video + (24 * 160);
        for (int j = 0; j < 160; j += 2) {
            ultima_fila[j] = ' ';
            ultima_fila[j + 1] = color_texto_actual;
        }
        cursor_fila = 24;
    }
}

void limpiar_pantalla() {
    char* memoria_video = (char*)0xB8000;
    for (int i = 0; i < 25 * 80 * 2; i += 2) {
        memoria_video[i] = ' ';
        memoria_video[i + 1] = color_texto_actual;
    }
    cursor_fila = 0;
    cursor_columna = 0;
    actualizar_cursor();
}

void establecer_color_vga(unsigned char texto, unsigned char fondo) {
    color_texto_actual = texto | (fondo << 4);
}

void kputc(char c) {
    char* memoria_video = (char*)0xB8000;
    int indice = (cursor_fila * 80 + cursor_columna) * 2;
    if (c == '\n') {
        cursor_columna = 0;
        cursor_fila++;
        hacer_scroll();
        actualizar_cursor();
        return;
    }
    if (c == '\b') {
        if (cursor_columna > 0) cursor_columna--;
        indice = (cursor_fila * 80 + cursor_columna) * 2;
        memoria_video[indice] = ' ';
        memoria_video[indice + 1] = color_texto_actual;
        actualizar_cursor();
        return;
    }
    memoria_video[indice] = c;
    memoria_video[indice + 1] = color_texto_actual;
    cursor_columna++;
    if (cursor_columna >= 80) {
        cursor_columna = 0;
        cursor_fila++;
    }
    hacer_scroll();
    actualizar_cursor();
}

void kprintf(const char* texto, ...) {
    int i = 0;
    while (texto[i] != '\0') {
        kputc(texto[i]);
        i++;
    }
}

void kprints_en(const char* texto, int fila, int columna) {
    if (fila >= 0 && fila < 25) cursor_fila = fila;
    if (columna >= 0 && columna < 80) cursor_columna = columna;
    kprintf(texto);
}

void dibujar_linea_horizontal(int fila, char caracter) {
    int col_orig = cursor_columna;
    int fil_orig = cursor_fila;
    cursor_fila = fila;
    for (int i = 0; i < 80; i++) {
        cursor_columna = i;
        kputc(caracter);
    }
    cursor_fila = fil_orig;
    cursor_columna = col_orig;
    actualizar_cursor();
}