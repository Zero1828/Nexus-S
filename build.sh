#!/bin/bash
# export PATH="$PATH:/c/Users/ANDRES DUVAN/Nexus-S/i686-elf-tools-windows/bin"
echo "==================================================="
echo "   COMPILANDO EL NUCLEO NEXUS-S EN MSYS2 🗿"
echo "==================================================="
echo

# 1. Crear la carpeta limpia para los archivos objetos
mkdir -p build

# 2. Compilar el archivo de arranque (Buscándolo dentro de la carpeta boot/)
echo "[ASM] Compilando boot/boot.asm..."
nasm -f win64 boot/boot.asm -o build/boot.o
if [ $? -ne 0 ]; then
    echo "☠️ Error al compilar boot.asm"
    exit 1
fi

# 3. Compilar el archivo principal del kernel (Ahora dentro de la carpeta kernel/)
echo "[C] Compilando kernel/kernel.c..."
x86_64-w64-mingw32-gcc -c kernel/kernel.c -o build/kernel.o -ffreestanding -O2 -Wall -Wextra -m64 -mno-red-zone -fno-pie
if [ $? -ne 0 ]; then
    echo "☠️ Error al compilar kernel.c"
    exit 1
fi

# 4. Bucle para compilar tus 48 controladores dentro de la carpeta kernel/
echo "[C] Compilando los 48 controladores y modulos..."
for f in kernel/*.c; do
if [ "$(basename "$f")" != "kernel.c" ] && [ -f "$f" ]; then
        filename=$(basename "$f" .c)
        echo "     - Compilando modulo: $f"
        x86_64-w64-mingw32-gcc -ffreestanding -fno-stack-protector -mno-red-zone -c "$f" -o "build/${filename%.c}.o"
        if [ $? -ne 0 ]; then
            echo "☠️ Error al compilar el modulo $f"
            exit 1
        fi
    fi
done

# 5. Enlazar todo pasando la lista completa de archivos objetos directamente
echo
echo "[LINKER] Uniendo el nucleo y los modulos en el binario final..."
# OJO A ESTA LÍNEA REESCRITA: Le pasa explícitamente todos los archivos .o de la carpeta build
/ucrt64/bin/ld.exe -T linker.ld -o build/kernel.tmp --gc-sections build/boot.o build/kernel.o build/*.o
/ucrt64/bin/objcopy.exe -O binary build/kernel.tmp build/kernel.bin
if [ $? -ne 0 ]; then
    echo "☠️ Error en el Linker"
    exit 1
fi
nasm -f bin boot/boot.asm -o boot_puro.bin
echo "[BIOS] Sector de arranque puro de 512 bytes generado en boot_puro.bin"
cat boot_puro.bin build/kernel.bin > mi_sistema_operativo.bin

echo
echo "==================================================="
echo "   ¡NEXUS-S COMPILADO CON EXITO EN MSYS2! 🚀"
echo "==================================================="
echo "Tu nucleo independiente esta listo en: mi_sistema_operativo.bin"
echo