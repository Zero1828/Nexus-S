// interfaz_media.c

// Definición de colores estándar de la matriz de texto VGA
#define COLOR_NEGRO         0x0
#define COLOR_AZUL          0x1
#define COLOR_VERDE         0x2
#define COLOR_CIAN          0x3
#define COLOR_ROJO          0x4
#define COLOR_GRIS_CLARO    0x7
#define COLOR_GRIS_OSCURO   0x8
#define COLOR_BLANCO        0xF

// Puntero directo a la memoria de video en modo texto (80 columnas x 25 filas)
static unsigned short* memoria_video_media = (unsigned short*)0xB8000;

/**
 * Pinta un solo carácter con colores en una coordenada X, Y fija
 */
void pintar_caracter_media(int x, int y, char caracter, unsigned char color_letra, unsigned char color_fondo) {
    if (x >= 80 || y >= 25 || x < 0 || y < 0) return;
    unsigned char atributo = (color_fondo << 4) | (color_letra & 0x0F);
    int indice = (y * 80) + x;
    memoria_video_media[indice] = caracter | (atributo << 8);
}

/**
 * Rellena toda la pantalla con un color de fondo plano
 */
void limpiar_pantalla_color(unsigned char color_fondo) {
    for (int y = 0; y < 25; y++) {
        for (int x = 0; x < 80; x++) {
            pintar_caracter_media(x, y, ' ', COLOR_BLANCO, color_fondo);
        }
    }
}

/**
 * Dibuja una ventana visual usando los caracteres extendidos de la BIOS
 */
void dibujar_ventana(int x, int y, int ancho, int alto, char* titulo, unsigned char color_borde, unsigned char color_fondo) {
    // 1. Rellenar el fondo interior de la ventana
    for (int fila = y + 1; fila < y + alto - 1; fila++) {
        for (int col = x + 1; col < x + ancho - 1; col++) {
            pintar_caracter_media(col, fila, ' ', COLOR_BLANCO, color_fondo);
        }
    }

    // 2. Dibujar las esquinas del marco (Códigos ASCII extendidos: ╔, ╗, ╚, ╝)
    pintar_caracter_media(x, y, 201, color_borde, color_fondo);                 
    pintar_caracter_media(x + ancho - 1, y, 187, color_borde, color_fondo);         
    pintar_caracter_media(x, y + alto - 1, 200, color_borde, color_fondo);         
    pintar_caracter_media(x + ancho - 1, y + alto - 1, 188, color_borde, color_fondo); 

    // 3. Dibujar bordes horizontales (═) y verticales (║)
    for (int i = x + 1; i < x + ancho - 1; i++) {
        pintar_caracter_media(i, y, 205, color_borde, color_fondo);             
        pintar_caracter_media(i, y + alto - 1, 205, color_borde, color_fondo); 
    }
    for (int i = y + 1; i < y + alto - 1; i++) {
        pintar_caracter_media(x, i, 186, color_borde, color_fondo);             
        pintar_caracter_media(x + ancho - 1, i, 186, color_borde, color_fondo); 
    }

    // 4. Imprimir el título en la barra superior de la ventana
    if (titulo != 0) {
        int i = 0;
        while (titulo[i] != '\0' && (x + 2 + i) < (x + ancho - 2)) {
            pintar_caracter_media(x + 2 + i, y, titulo[i], COLOR_BLANCO, color_borde);
            i++;
        }
    }
}

/**
 * FUNCIÓN CLAVE: Un kprintf propio de la interfaz que recibe posición X, Y
 * Soporta formateo básico de cadenas (%s) y números enteros (%d)
 */
void kprintf_en(int x, int y, unsigned char color_letra, unsigned char color_fondo, const char* formato, ...) {
    // Puntero para recorrer los argumentos variables (simulación manual de va_list para evitar dependencias)
    __builtin_va_list args;
    __builtin_va_start(args, formato);

    int posX = x;
    int posY = y;

    for (int i = 0; formato[i] != '\0'; i++) {
        if (formato[i] == '%' && formato[i+1] != '\0') {
            i++; // Avanzar al especificador
            
            if (formato[i] == 's') {
                // Formatear cadenas de texto
                char* string_arg = __builtin_va_arg(args, char*);
                while (*string_arg != '\0') {
                    pintar_caracter_media(posX++, posY, *string_arg++, color_letra, color_fondo);
                }
            } 
            else if (formato[i] == 'd') {
                // Formatear números enteros
                int num_arg = __builtin_va_arg(args, int);
                if (num_arg == 0) {
                    pintar_caracter_media(posX++, posY, '0', color_letra, color_fondo);
                } else {
                    if (num_arg < 0) {
                        pintar_caracter_media(posX++, posY, '-', color_letra, color_fondo);
                        num_arg = -num_arg;
                    }
                    char buffer_num[12];
                    int idx = 0;
                    while (num_arg > 0) {
                        buffer_num[idx++] = (num_arg % 10) + '0';
                        num_arg /= 10;
                    }
                    for (int j = idx - 1; j >= 0; j--) {
                        pintar_caracter_media(posX++, posY, buffer_num[j], color_letra, color_fondo);
                    }
                }
            }
        } else {
            // Imprimir carácter normal
            pintar_caracter_media(posX++, posY, formato[i], color_letra, color_fondo);
        }
    }
    __builtin_va_end(args);
}

/**
 * Función principal que llama tu Kernel para arrancar este modo
 */
void lanzar_interfaz_media() {
    // 1. Fondo azul de "Escritorio amigable"
    limpiar_pantalla_color(COLOR_AZUL);
    
    // 2. Ventana principal de la Shell asistida
    dibujar_ventana(8, 2, 64, 19, " Terminal Asistida Nexus-S ", COLOR_GRIS_CLARO, COLOR_NEGRO);
    
    // 3. Barra de tareas inferior (Gris con texto negro)
    for (int x = 0; x < 80; x++) {
        pintar_caracter_media(x, 24, ' ', COLOR_NEGRO, COLOR_GRIS_CLARO);
    }
    kprintf_en(2, 24, COLOR_NEGRO, COLOR_GRIS_CLARO, "[ F1 ] Menu  |  [ F2 ] Ejecutar  |  Modo: MEDIO");

    // 4. Imprimir textos informativos formateados con kprintf_en dentro de la ventana negra
    kprintf_en(10, 4, COLOR_VERDE, COLOR_NEGRO, "Nucleo operativo: Nexus-S [ ONLINE ]");
    kprintf_en(10, 5, COLOR_GRIS_CLARO, COLOR_NEGRO, "Bienvenido al entorno amigable basado en texto.");
    
    // Ejemplo de uso de formateo con %s y %d
    char* estado_usuario = "Administrador";
    int revision_build = 104;
    kprintf_en(10, 7, COLOR_CIAN, COLOR_NEGRO, "Usuario actual: %s  |  Compilacion: #%d", estado_usuario, revision_build);
    
    kprintf_en(10, 9, COLOR_BLANCO, COLOR_NEGRO, "nexus-s@user_media:~$ _");
    
}