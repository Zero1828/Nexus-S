bits 16
global loader

loader:
    ; Configurar los segmentos de datos (Importante para que encuentre los textos)
    mov ax, 0x07C0
    mov ds, ax
    mov es, ax

    mov si, msg_loading
print_loading:
    lodsb
    or al, al          ; Verifica si el caracter es 0 (Fin de la cadena)
    jz start_main
    mov ah, 0x0E       ; Función para imprimir caracter en BIOS
    int 0x10
    jmp print_loading

start_main:
    mov cx, 0xFFFF
delay:
    loop delay

    ; Limpiar pantalla y configurar modo de video de texto estándar
    mov ax, 0x0003
    int 0x10

    mov si, msg_title
print_title:
    lodsb
    or al, al
    jz near leer_teclado
    mov ah, 0x0E
    mov bl, 0x01       ; Color azul (en algunos modos BIOS)
    int 0x10
    jmp print_title

print_welcome:
    mov si, msg_welcome
print_wel_loop:
    lodsb
    or al, al
    jz near leer_teclado
    mov ah, 0x0E
    mov bl, 0x0F       ; Color blanco
    int 0x10
    jmp print_wel_loop


leer_teclado:
    mov ah, 0x00        ; Leer tecla del teclado
    int 0x16            ; AL recibe el carácter ASCII
    mov ah, 0x0e        ; Función teletipo para imprimir
    int 0x10            ; Imprimir el carácter en pantalla
    jmp leer_teclado          ; Volver a esperar la siguiente tecla

; ; Cadenas de texto en formato NASM... (línea 52 en adelante se queda igual)

; Cadenas de texto en formato NASM (db = Define Byte, terminado en 0)
msg_loading: db "Loading System...", 0
msg_title:   db "[ NEXUS-S ]", 13, 10, 0    ; 13, 10 es \r\n
msg_welcome: db "Welcome!", 13, 10, 0

; Firma de arranque obligatoria para que la BIOS sepa que es un Sistema Operativo
times 510-($-$$) db 0   ; Rellena con ceros hasta el byte 510
dw 0xAA55               ; Palabra mágica de arranque en el byte 511 y 512