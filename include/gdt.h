//
// gdt.h - stuff for GDT setup
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

#ifndef _GDT_H
#define _GDT_H

#include <stdint.h>

#define _GDT_ACC_PRESENT    ((1<<7)|(1<<4))
#define _GDT_ACC_KPRIV      (0<<5)  /* kernel access level */
#define _GDT_ACC_UPRIV      (3<<5)  /* user access level */
#define _GDT_ACC_EXECUTABLE (1<<3)  /* 1=code, 0=data */
#define _GDT_ACC_CONFORMING (1<<2)  /* this is a tricky one */
                            //  for code segments:
                            //      if conforming can be executed by lower 
                            //      privilege
                            //  for data segments:
                            //      1=segment grows down
                            //      0=segment grows up
                            //      should use 0, bcs we have only one data segm
                            //      for a mode anyways
#define _GDT_ACC_RW         (1<<1)  /* for code: 1=readonly,0=no r/w */
                                    // for data: 1=r/w,0=readonly
#define _GDT_ACC_ACCESSED   (1<<0)  /* this is set by proc when segm accessed */

#define _GDT_GRAN_GRAN      (1<<7)  /* 1=>[limit]=1B,0=>[limit]=4KiB */
#define _GDT_GRAN_SIZE      (1<<6)  /* 1=32-bit,0=16-bit */

#define _GDT_ACC_K_TEXT     _GDT_ACC_PRESENT | _GDT_ACC_KPRIV | \
            _GDT_ACC_EXECUTABLE | _GDT_ACC_RW
#define _GDT_ACC_K_DATA     _GDT_ACC_PRESENT | _GDT_ACC_KPRIV | _GDT_ACC_RW

#define _GDT_GRAN_FLAGS     _GDT_GRAN_GRAN | _GDT_GRAN_SIZE

#define _GDT_KERNEL_TEXT    0x08
#define _GDT_KERNEL_DATA    0x10

struct gdt_entry_struct
{
    uint16_t    limit_lo;           // lowest 16 bits of limit
    uint16_t    base_lo;            // lowest 16 bits of base
    uint8_t     base_mid;           // next 8 bits of base
    uint8_t     access_flags;
    uint8_t     granularity_flags;  // also 4 highest bits of limit
    uint8_t     base_hi;            // highest 8 bits of base
}__attribute__((packed));
typedef struct gdt_entry_struct gdt_entry_t;

struct gdt_table_struct
{
    uint16_t    limit;  //  upper 16 bits of all selector limits
    uint32_t    base;   //  address of first gdt_entry_t in table
}__attribute__((packed));
typedef struct gdt_table_struct gdt_table_t;

void    gdt_init();

#endif
