;
; boot.s - kernel start loc
;
; MIT License
; 
; Copyright (c) 2017 Uko Kokņevičs (Uko Koknevics)
; 
; Permission is hereby granted, free of charge, to any person obtaining a copy
; of this software and associated documentation files (the "Software"), to deal
; in the Software without restriction, including without limitation the rights
; to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
; copies of the Software, and to permit persons to whom the Software is
; furnished to do so, subject to the following conditions:
; 
; The above copyright notice and this permission notice shall be included in all
; copies or substantial portions of the Software.
; 
; THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
; IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
; FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
; AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
; LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
; OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
; SOFTWARE.
; 

;MBOOT_PAGE_ALIGN    equ 1<<0       ;   all modules on page boundaries
                                    ;   doesn't matter since we don't use paging
                                    ;   and don't have any modules
;MBOOT_MEM_INFO      equ 1<<1       ;   we don't need meminfo since we don't use
                                    ;   dynamic memory
MBOOT_HEADER_MAGIC  equ 0x1BADB002  ;   multiboot standard special magic code
MBOOT_HEADER_FLAGS  equ 0           ;   MBOOT_PAGE_ALIGN | MBOOT_MEM_INFO
MBOOT_CHECKSUM      equ -(MBOOT_HEADER_MAGIC + MBOOT_HEADER_FLAGS)

[BITS 32]                           ;   We use 32 bits since there's no real
                                    ;   reason for 64 bits
                                    ;   also 32bit => more machines

[GLOBAL mboot]
[EXTERN code]
[EXTERN bss]
[EXTERN end]

mboot:                              ;   here we put down all the flags
    dd MBOOT_HEADER_MAGIC
    dd MBOOT_HEADER_FLAGS
    dd MBOOT_CHECKSUM
    
    dd mboot                        ;   iirc multiboot needed this (actually dk)
    dd code
    dd bss
    dd end
    dd start

[GLOBAL start]
[EXTERN main]

start:
    ; push ebx                      ; since we didn't pass MBOOT_MEM_INFO
                                    ; we don't need this
    
    cli                             ; disable interrupts
    call main
    jmp $                           ; in case we exit from main (we shouldn't)
                                    ; we have an infinite loop here
