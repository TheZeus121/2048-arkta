//
// gdt.c - implementation of gdt.h
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

#include <gdt.h>
#include <stdint.h>
#include <stdio.h>

// in tables.s
extern void lgdt(uint32_t);

gdt_entry_t gdt_entries[5];
gdt_table_t gdt_table;

static void gdt_set_entry(uint32_t index, uint32_t base, uint32_t limit, 
                          uint8_t access, uint8_t granularity);

void gdt_init()
{
    printf("Loading GDT... ");
    gdt_table.limit = (sizeof(gdt_entry_t) * 5) - 1;
    gdt_table.base  = (uint32_t) &gdt_entries;
    
    gdt_set_entry(0, 0, 0, 0, 0);
    gdt_set_entry(1, 0, 0xFFFFF, _GDT_ACC_K_TEXT, _GDT_GRAN_FLAGS); // 0x08
    gdt_set_entry(2, 0, 0xFFFFF, _GDT_ACC_K_DATA, _GDT_GRAN_FLAGS); // 0x10
    
    lgdt((uint32_t) &gdt_table);
    printf("Done!\n");
}

static void gdt_set_entry(uint32_t index, uint32_t base, uint32_t limit, 
                          uint8_t access, uint8_t granularity)
{
    gdt_entries[index].base_lo          = (base & 0xFFFF);
    gdt_entries[index].base_mid         = (base >> 16) & 0xFF;
    gdt_entries[index].base_hi          = (base >> 24) & 0xFF;
    
    gdt_entries[index].limit_lo         = (limit & 0xFFFF);
    gdt_entries[index].granularity_flags= (limit >> 16) & 0xF;
    
    gdt_entries[index].granularity_flags |= granularity & 0xF0;
    gdt_entries[index].access_flags       = access;
}
