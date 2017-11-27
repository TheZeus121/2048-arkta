//
// irq.c - implementation of irq.h
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

#include <stdint.h>
#include <stdio.h>

#include "gdt.h"
#include "idt.h"
#include "irq.h"
#include "ports.h"
#include "ps2.h"

//
// List of IRQs:
//  0  - Programmable Interrupt Timer
//  1  - PS/2 first port
//  2  - Cascade, NEVER raised
//  3  - COM2
//  4  - COM1
//  5  - LPT2
//  6  - Floppy Disk
//  7  - LPT1 / unreliable spurious interrupt
//  8  - CMOS real-time clock
//  9  - free for peripherals / legacy SCSI / NIC
//  10 - free for peripherals / SCSI / NIC
//  11 - free for peripherals / SCSI / NIC
//  12 - PS/2 second port
//  13 - FPU / Coprocessor / Inter-processor
//  14 - Primary ATA Hard Disk
//  15 - Secondary ATA Hard Disk / unreliable spurious interrupt
//

void handler(regs_t* regs)
{
    //printf("IRQ %i\n", regs->int_no - 32);
    int irq_num = regs->int_no - 0x20;
    
    if(irq_num == 7) // maybe spurious int?
    {
        outb(_PIC_MASTER_COMMAND, _PIC_READ_ISR);
        io_wait();
        int tmp = inb(_PIC_MASTER_COMMAND);
        if((tmp & (1<<7)) != (1<<7))
            return;
    }
    else if(irq_num == 15) // the same but for slave
    {
        outb(_PIC_SLAVE_COMMAND, _PIC_READ_ISR);
        io_wait();
        int tmp = inb(_PIC_SLAVE_COMMAND);
        if((tmp & (1<<7)) != (1<<7))
        {
            // bcs master doesn't know that slave fucked up
            outb(_PIC_MASTER_COMMAND, _PIC_EOI);
            return;
        }
    }
    
    switch(irq_num)
    {
        case 1:     // PS/2 first
            ps2_first();
            break;
        case 12:    // PS/2 second
            ps2_second();
            break;
        default:
            printf("\nIRQ%d\n", irq_num);
            break;
    }
    
    if(irq_num >= 8)
        outb(_PIC_SLAVE_COMMAND, _PIC_EOI);
    outb(_PIC_MASTER_COMMAND, _PIC_EOI);
}

void irq_init()
{
    printf("Initializing IRQs... ");
    // first we gotta remap PIC
    // start initializing
    outb(_PIC_MASTER_COMMAND, _PIC_INIT);
    io_wait();
    outb(_PIC_SLAVE_COMMAND,  _PIC_INIT);
    io_wait();
    
    // make the irqs start from 0x20
    outb(_PIC_MASTER_DATA, 0x20);
    io_wait();
    outb(_PIC_SLAVE_DATA,  0x28);
    io_wait();
    
    // slave is on 00000100 pin
    outb(_PIC_MASTER_DATA, 4);
    io_wait();
    
    // you're the second
    outb(_PIC_SLAVE_DATA,  2);
    io_wait();
    
    // use 8086 mode
    outb(_PIC_MASTER_DATA, _PIC_8086_MODE);
    io_wait();
    outb(_PIC_SLAVE_DATA,  _PIC_8086_MODE);
    io_wait();
    
    // mask everything
    outb(_PIC_MASTER_DATA, 0xFF);
    outb(_PIC_SLAVE_DATA,  0xFF);
    
    for(int i = 0x20; i < 0x30; i++)
        idt_register_interrupt(i, handler);
    
    outb(_PIC_SLAVE_COMMAND,  _PIC_EOI);
    outb(_PIC_MASTER_COMMAND, _PIC_EOI);
    printf("Done!\n");
}

void irq_enable(int i)
{
    if(i >= 8)
    {
        i -= 8;
        uint8_t tmp = inb(_PIC_SLAVE_DATA);
        tmp &= ~(1<<i);
        outb(_PIC_SLAVE_DATA, tmp);
        i = 2; // also enable cascade on master
    }
    uint8_t tmp = inb(_PIC_MASTER_DATA);
    tmp &= ~(1<<i);
    outb(_PIC_MASTER_DATA, tmp);
}

void irq_disable(int i)
{
    if(i >= 8)
    {
        i -= 8;
        uint8_t tmp = inb(_PIC_SLAVE_DATA);
        tmp |= 1<<i;
        outb(_PIC_SLAVE_DATA, tmp);
        return; 
        // don't wanna disable cascade on master bcs myb something uses it
    }
    uint8_t tmp = inb(_PIC_MASTER_DATA);
    tmp |= 1<<i;
    outb(_PIC_MASTER_DATA, tmp);
}
