@echo off
title Compilador Nexus-S
cls

echo ===================================================
echo   COMPILANDO EL NUCLEO NEXUS-S 🗿
echo ===================================================
echo.

:: 1. Crear la carpeta limpia para los archivos objetos
if not exist build mkdir build

:: 2. Compilar el archivo de arranque (Ensamblador)
echo [ASM] Compilando boot.asm...
nasm -f elf32 boot.asm -o build/boot.o
if %errorlevel% neq 0 goto :error

:: 3. Compilar el archivo principal del kernel
echo [C] Compilando kernel.c...
i686-elf-gcc -c kernel.c -o build/kernel.o -ffreestanding -O2 -Wall -Wextra
if %errorlevel% neq 0 goto :error

:: 4. Bucle para compilar tus 48 archivos .c automáticamente
echo [C] Compilando los 48 controladores y modulos...
for %%f in (*.c) do (
    if not "%%f"=="kernel.c" (
        echo     - Compilando modulo: %%f
        i686-elf-gcc -c "%%f" -o "build/%%~nf.o" -ffreestanding -O2 -Wall -Wextra
        if %errorlevel% neq 0 goto :error
    )
)

:: 5. Enlazar todo usando tu linker.ld limpio
echo.
echo [LINKER] Uniendo el nucleo y los 48 modulos en el binario final...
i686-elf-ld -T linker.ld -o mi_sistema_operativo.bin
if %errorlevel% neq 0 goto :error

echo.
echo ===================================================
echo   ¡NEXUS-S COMPILADO CON EXITO! 🚀
echo ===================================================
echo Tu nucleo independiente esta listo en: mi_sistema_operativo.bin
echo.
pause
exit

:error
echo.
echo ***************************************************
echo   [ERROR] El modo hardcore ha fallado. ☠️
echo ***************************************************
echo Revisa los mensajes de error de la consola.
echo.
pause
exit