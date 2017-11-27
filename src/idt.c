//
// idt.c - implementation of idt.h
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

#include "ports.h"
#include <idt.h>
#include <gdt.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

const char reserved[] = "Intel(R) Reserved";

const char* error_msgs[] = 
{
    "#DE Divide Error",                     // 0
    "#DB Debug Exception",                  // 1
    "NMI Interrupt",                        // 2
    "#BP Breakpoint",                       // 3
    "#OF Overflow",                         // 4
    "#BR BOUND Range Exceeded",             // 5
    "#UD Undefined Opcode",                 // 6
    "#NM No Math Coprocessor",              // 7
    "#DF Double Float",                     // 8
    reserved,                               // 9
    "#TS Invalid TSS",                      // 10
    "#NP Segment Not Present",              // 11
    "#SS Stack-Segment Fault",              // 12
    "#GP General Protection",               // 13
    "#PF Page Fault",                       // 14
    reserved,                               // 15
    reserved,                               // 16
    "#AC Alignment Check",                  // 17
    "#MC Machine Check",                    // 18
    "#XM SIMD Floating-Point Exception",    // 19
    "#VE Virtualization Exception",         // 20
    reserved,                               // 21
    reserved,                               // 22
    reserved,                               // 23
    reserved,                               // 24
    reserved,                               // 25
    reserved,                               // 26
    reserved,                               // 27
    reserved,                               // 28
    reserved,                               // 29
    reserved,                               // 30
    reserved                                // 31
};

// in lidt.s
extern void lidt(uint32_t);

idt_entry_t idt_entries[256];
idt_table_t idt_table;

int_handler_t int_handlers[256];

void idt_init()
{
    printf("Loading IDT... ");
    idt_table.limit = sizeof(idt_entry_t) * 256 - 1;
    idt_table.base  = (uint32_t) &idt_entries;
    
    memset(&idt_entries,  0, sizeof(idt_entry_t) * 256);
    memset(&int_handlers, 0, sizeof(int_handler_t) * 256);
    
    // Intel(R) Exceptions
    // 0x00
    idt_set_entry(0 , (uint32_t)int0 , _GDT_KERNEL_TEXT, _IDT_INTERRUPT_K);
    idt_set_entry(1 , (uint32_t)int1 , _GDT_KERNEL_TEXT, _IDT_INTERRUPT_K);
    idt_set_entry(2 , (uint32_t)int2 , _GDT_KERNEL_TEXT, _IDT_INTERRUPT_K);
    idt_set_entry(3 , (uint32_t)int3 , _GDT_KERNEL_TEXT, _IDT_INTERRUPT_K);
    idt_set_entry(4 , (uint32_t)int4 , _GDT_KERNEL_TEXT, _IDT_INTERRUPT_K);
    idt_set_entry(5 , (uint32_t)int5 , _GDT_KERNEL_TEXT, _IDT_INTERRUPT_K);
    idt_set_entry(6 , (uint32_t)int6 , _GDT_KERNEL_TEXT, _IDT_INTERRUPT_K);
    idt_set_entry(7 , (uint32_t)int7 , _GDT_KERNEL_TEXT, _IDT_INTERRUPT_K);
    idt_set_entry(8 , (uint32_t)int8 , _GDT_KERNEL_TEXT, _IDT_INTERRUPT_K);
    idt_set_entry(9 , (uint32_t)int9 , _GDT_KERNEL_TEXT, _IDT_INTERRUPT_K);
    idt_set_entry(10, (uint32_t)int10, _GDT_KERNEL_TEXT, _IDT_INTERRUPT_K);
    idt_set_entry(11, (uint32_t)int11, _GDT_KERNEL_TEXT, _IDT_INTERRUPT_K);
    idt_set_entry(12, (uint32_t)int12, _GDT_KERNEL_TEXT, _IDT_INTERRUPT_K);
    idt_set_entry(13, (uint32_t)int13, _GDT_KERNEL_TEXT, _IDT_INTERRUPT_K);
    idt_set_entry(14, (uint32_t)int14, _GDT_KERNEL_TEXT, _IDT_INTERRUPT_K);
    idt_set_entry(15, (uint32_t)int15, _GDT_KERNEL_TEXT, _IDT_INTERRUPT_K);
    // 0x10
    idt_set_entry(16, (uint32_t)int16, _GDT_KERNEL_TEXT, _IDT_INTERRUPT_K);
    idt_set_entry(17, (uint32_t)int17, _GDT_KERNEL_TEXT, _IDT_INTERRUPT_K);
    idt_set_entry(18, (uint32_t)int18, _GDT_KERNEL_TEXT, _IDT_INTERRUPT_K);
    idt_set_entry(19, (uint32_t)int19, _GDT_KERNEL_TEXT, _IDT_INTERRUPT_K);
    idt_set_entry(20, (uint32_t)int20, _GDT_KERNEL_TEXT, _IDT_INTERRUPT_K);
    idt_set_entry(21, (uint32_t)int21, _GDT_KERNEL_TEXT, _IDT_INTERRUPT_K);
    idt_set_entry(22, (uint32_t)int22, _GDT_KERNEL_TEXT, _IDT_INTERRUPT_K);
    idt_set_entry(23, (uint32_t)int23, _GDT_KERNEL_TEXT, _IDT_INTERRUPT_K);
    idt_set_entry(24, (uint32_t)int24, _GDT_KERNEL_TEXT, _IDT_INTERRUPT_K);
    idt_set_entry(25, (uint32_t)int25, _GDT_KERNEL_TEXT, _IDT_INTERRUPT_K);
    idt_set_entry(26, (uint32_t)int26, _GDT_KERNEL_TEXT, _IDT_INTERRUPT_K);
    idt_set_entry(27, (uint32_t)int27, _GDT_KERNEL_TEXT, _IDT_INTERRUPT_K);
    idt_set_entry(28, (uint32_t)int28, _GDT_KERNEL_TEXT, _IDT_INTERRUPT_K);
    idt_set_entry(29, (uint32_t)int29, _GDT_KERNEL_TEXT, _IDT_INTERRUPT_K);
    idt_set_entry(30, (uint32_t)int30, _GDT_KERNEL_TEXT, _IDT_INTERRUPT_K);
    idt_set_entry(31, (uint32_t)int31, _GDT_KERNEL_TEXT, _IDT_INTERRUPT_K);
    
    // PIC IRQs
    // 0x20
    idt_set_entry(32, (uint32_t)int32, _GDT_KERNEL_TEXT, _IDT_INTERRUPT_K);
    idt_set_entry(33, (uint32_t)int33, _GDT_KERNEL_TEXT, _IDT_INTERRUPT_K);
    idt_set_entry(34, (uint32_t)int34, _GDT_KERNEL_TEXT, _IDT_INTERRUPT_K);
    idt_set_entry(35, (uint32_t)int35, _GDT_KERNEL_TEXT, _IDT_INTERRUPT_K);
    idt_set_entry(36, (uint32_t)int36, _GDT_KERNEL_TEXT, _IDT_INTERRUPT_K);
    idt_set_entry(37, (uint32_t)int37, _GDT_KERNEL_TEXT, _IDT_INTERRUPT_K);
    idt_set_entry(38, (uint32_t)int38, _GDT_KERNEL_TEXT, _IDT_INTERRUPT_K);
    idt_set_entry(39, (uint32_t)int39, _GDT_KERNEL_TEXT, _IDT_INTERRUPT_K);
    idt_set_entry(40, (uint32_t)int40, _GDT_KERNEL_TEXT, _IDT_INTERRUPT_K);
    idt_set_entry(41, (uint32_t)int41, _GDT_KERNEL_TEXT, _IDT_INTERRUPT_K);
    idt_set_entry(42, (uint32_t)int42, _GDT_KERNEL_TEXT, _IDT_INTERRUPT_K);
    idt_set_entry(43, (uint32_t)int43, _GDT_KERNEL_TEXT, _IDT_INTERRUPT_K);
    idt_set_entry(44, (uint32_t)int44, _GDT_KERNEL_TEXT, _IDT_INTERRUPT_K);
    idt_set_entry(45, (uint32_t)int45, _GDT_KERNEL_TEXT, _IDT_INTERRUPT_K);
    idt_set_entry(46, (uint32_t)int46, _GDT_KERNEL_TEXT, _IDT_INTERRUPT_K);
    idt_set_entry(47, (uint32_t)int47, _GDT_KERNEL_TEXT, _IDT_INTERRUPT_K);
    
    lidt((uint32_t) &idt_table);
    printf("Done!\n");
}

void idt_set_entry(uint8_t index, uint32_t base, uint16_t segment, 
                          uint8_t flags)
{
    idt_entries[index].base_lo = base & 0xFFFF;
    idt_entries[index].base_hi = (base >> 16) & 0xFFFF;
    
    idt_entries[index].segment = segment;
    idt_entries[index].always0 = 0; // lol
    idt_entries[index].flags = flags;
}

void idt_exception(regs_t* regs)
{
    if(regs->int_no < 32)
    {
        printf("\nException: %i %s\n", regs->int_no, error_msgs[regs->int_no]);
        switch(regs->int_no)
        {
            case 8:
            case 10:
            case 11:
            case 12:
            case 13:
            case 17:
                printf("Error code: %i\n", regs->err_code);
                break;
            case 14:    //  Page Fault
                printf("Information about error: ");
                if(regs->err_code & (1<<0))
                    printf("Page was present\n");
                else
                    printf("Page wasn't present\n");
                if(regs->err_code & (1<<1))
                    printf("Caused by a write\n");
                else
                    printf("Caused by a read\n");
                if(regs->err_code & (1<<2))
                    printf("While in user-mode\n");
                else
                    printf("While in kernel-mode\n");
                if(regs->err_code & (1<<3))
                    printf("Reserved bits were owerwritten\n");
                if(regs->err_code & (1<<4))
                    printf("While fetching an instruction\n");
        }
        for(;;);
    }
    else if(int_handlers[regs->int_no] != 0)
    {
        int_handler_t handler = int_handlers[regs->int_no];
        handler(regs);
    }
    else
    {
        printf("\nUnkown interrupt occured: INT %i\n", regs->int_no);
    }
}

void idt_register_interrupt(uint8_t num, int_handler_t int_handler)
{
    int_handlers[num] = int_handler;
}
