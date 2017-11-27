[BITS 32]
[GLOBAL lgdt]

lgdt:
    mov eax, [esp+4]    ;   Get first argument
    lgdt [eax]          ;   Load GDT
    
    mov eax, 0x10
    mov ds, eax
    mov es, eax
    mov fs, eax
    mov gs, eax
    mov ss, eax
    jmp 0x08:.flush
.flush:
    ret
