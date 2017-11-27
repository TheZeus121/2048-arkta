[BITS 32]

[GLOBAL lidt]
lidt:
    mov eax, [esp+4]    ;   the pointer
    lidt [eax]          ;   to the table we load here
    ret

%macro INT_NOERR 1
[GLOBAL int%1]
int%1:
    cli
    push byte 0
    push byte %1
    jmp int_common
%endmacro

%macro INT_ERR 1
[GLOBAL int%1]
int%1:
    cli
    push byte %1
    jmp int_common
%endmacro

%macro IRQ 2
[GLOBAL irq%1]
irq%1:
    cli
    push byte 0
    push byte %2
    jmp int_common
%endmacro

; in idt.c
[EXTERN idt_exception]
int_common:         ;   we already have: cs, eip, eflags, ss, sp
                    ;   errorcode(byte)
                    ;   interrupt number(byte)
    pusha           ;   edi, esi, ebp, esp, ebx, edx, ecx, eax
    push ds
    push es
    push fs
    push gs
    
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    mov eax, esp
    push esp
    
    mov eax, idt_exception
    call eax
    
    pop eax
    pop gs
    pop fs
    pop es
    pop ds
    popa
    add esp, 8
    sti
    iret

; Exceptions 0x00-0x1F
INT_NOERR   0
INT_NOERR   1
INT_NOERR   2
INT_NOERR   3
INT_NOERR   4
INT_NOERR   5
INT_NOERR   6
INT_NOERR   7
INT_ERR     8
INT_NOERR   9
INT_ERR     10
INT_ERR     11
INT_ERR     12
INT_ERR     13
INT_ERR     14
INT_NOERR   15
INT_NOERR   16
INT_ERR     17
INT_NOERR   18
INT_NOERR   19
INT_NOERR   20
INT_NOERR   21
INT_NOERR   22
INT_NOERR   23
INT_NOERR   24
INT_NOERR   25
INT_NOERR   26
INT_NOERR   27
INT_NOERR   28
INT_NOERR   29
INT_NOERR   30
INT_NOERR   31

; irqs 0x20-0x2F
INT_NOERR   32
INT_NOERR   33
INT_NOERR   34
INT_NOERR   35
INT_NOERR   36
INT_NOERR   37
INT_NOERR   38
INT_NOERR   39
INT_NOERR   40
INT_NOERR   41
INT_NOERR   42
INT_NOERR   43
INT_NOERR   44
INT_NOERR   45
INT_NOERR   46
INT_NOERR   47
