global gdt_flush

gdt_flush:
    lgdt [rdi]          ; En x86_64 System V AMD64 ABI, el primer argumento llega en RDI
    
    ; Recargar los selectores de datos con el segmento de datos del kernel (índice 2 = 0x10)
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    
    ; Salto lejano (far jump) para recargar el registro CS con el segmento de código (índice 1 = 0x08)
    push 0x08
    lea rax, [rel .reload_cs]
    push rax
    retfq

.reload_cs:
    ret