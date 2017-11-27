//
// ps2.h - driver interface for PS/2 controller
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

#ifndef __DRIVER_PS2_H
#define __DRIVER_PS2_H

#include <stdbool.h>
#include <stdint.h>

#include "common.h"

#define _PS2_TYPE_UNKNOWN               0
#define _PS2_TYPE_UNKNOWN_KB            1
#define _PS2_TYPE_TRANSLATED_AT_KB      2
#define _PS2_TYPE_TRANSLATED_MF2_KB     3
#define _PS2_TYPE_MF2_KB                4

struct device_struct
{
    bool    works;
    bool    failed;
    int     type;
    struct code_struct
    {
        int length;
        int code[3];
    } code;
};
typedef struct device_struct device_t;

struct ps2_interface_struct
{
    init_f      init;
    writeb_f    writeb;
};
typedef struct ps2_interface_struct ps2_interface_t;

void            ps2_init();
bool            ps2_status();

ps2_interface_t ps2_register(int port, recvb_f recvb);

void            ps2_first();
void            ps2_second();

#endif
