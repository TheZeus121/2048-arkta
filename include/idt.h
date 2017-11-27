//
// idt.h - stuff for IDT setup
//

/*******************************************************************************
* MIT License                                                                  *
*                                                                              *
* Copyright (c) 2017 Uko Kokņevičs (Uko Koknevics)                             *
*                                                                              *
* Permission is hereby granted, free of charge, to any person obtaining a copy *
* of this software and associated documentation files (the "Software"), to     *
* deal in the Software without restriction, including without limitation the   *
* rights to use, copy, modify, merge, publish, distribute, sublicense, and/or  *
* sell copies of the Software, and to permit persons to whom the Software is   *
* furnished to do so, subject to the following conditions:                     *
*                                                                              *
* The above copyright notice and this permission notice shall be included in   *
* all copies or substantial portions of the Software.                          *
*                                                                              *
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR   *
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,     *
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE  *
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER       *
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING      *
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS *
* IN THE SOFTWARE.                                                             *
*******************************************************************************/

#ifndef _IDT_H
#define _IDT_H

#include <stdint.h>

#define _IDT_FLAG_INTERRUPT ((1<<2)|(1<<1))
#define _IDT_FLAG_TRAP      ((1<<2)|(1<<1)|(1<<0))
#define _IDT_FLAG_SIZE      (1<<3)          /* 1=32-bit,0=16-bit */
#define _IDT_FLAG_K         (0<<5)
#define _IDT_FLAG_U         (3<<5)
#define _IDT_FLAG_PRES      (1<<7)

#define _IDT_INTERRUPT_K    _IDT_FLAG_INTERRUPT | _IDT_FLAG_SIZE | _IDT_FLAG_K \
                                                                | _IDT_FLAG_PRES
#define _IDT_TRAP_K         _IDT_FLAG_TRAP | _IDT_FLAG_SIZE | _IDT_FLAG_K \
                                                                | _IDT_FLAG_PRES
#define _IDT_INTERRUPT_U    _IDT_FLAG_INTERRUPT | _IDT_FLAG_SIZE | _IDT_FLAG_U \
                                                                | _IDT_FLAG_PRES
#define _IDT_TRAP_U         _IDT_FLAG_TRAP | _IDT_FLAG_SIZE | _IDT_FLAG_U \
                                                                | _IDT_FLAG_PRES

struct idt_entry_struct
{
    uint16_t    base_lo;    // lowest 16 bits of address
    uint16_t    segment;    // which kernel segment?
    uint8_t     always0;    // the name says it all
    uint8_t     flags;
    uint16_t    base_hi;    // upper 16 bits of address
}__attribute__((packed));
typedef struct idt_entry_struct idt_entry_t;

struct idt_table_struct
{
    uint16_t limit;
    uint32_t base;
}__attribute__((packed));
typedef struct idt_table_struct idt_table_t;

struct regs_struct
{
    uint32_t gs, fs, es, ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags, useresp, ss;
}__attribute__((packed));
typedef struct regs_struct regs_t;

// int_handler_t is a pointer to a function "void XXX(regs_t*)"
typedef void (*int_handler_t)(regs_t*);

void idt_init();

void idt_set_entry(uint8_t index, uint32_t base, uint16_t segment, 
                                                                uint8_t flags);

void idt_register_interrupt(uint8_t num, int_handler_t handler);

// all these bois are in lidt.s
// Intel(R) exceptions
extern void int0 (); // 0x00
extern void int1 ();
extern void int2 ();
extern void int3 ();
extern void int4 ();
extern void int5 ();
extern void int6 ();
extern void int7 ();
extern void int8 ();
extern void int9 ();
extern void int10();
extern void int11();
extern void int12();
extern void int13();
extern void int14();
extern void int15();
extern void int16(); // 0x10
extern void int17();
extern void int18();
extern void int19();
extern void int20();
extern void int21();
extern void int22();
extern void int23();
extern void int24();
extern void int25();
extern void int26();
extern void int27();
extern void int28();
extern void int29();
extern void int30();
extern void int31();

// PIC IRQs
extern void int32(); // 0x20
extern void int33();
extern void int34();
extern void int35();
extern void int36();
extern void int37();
extern void int38();
extern void int39();
extern void int40();
extern void int41();
extern void int42();
extern void int43();
extern void int44();
extern void int45();
extern void int46();
extern void int47();

#endif
