//
// irq.h - Interrupt Request Handlers
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

#ifndef _IRQ_H
#define _IRQ_H

#define _IRQ_0  32
#define _IRQ_1  33
#define _IRQ_2  34
#define _IRQ_3  35
#define _IRQ_4  36
#define _IRQ_5  37
#define _IRQ_6  38
#define _IRQ_7  39
#define _IRQ_8  40
#define _IRQ_9  41
#define _IRQ_10 42
#define _IRQ_11 43
#define _IRQ_12 44
#define _IRQ_13 45
#define _IRQ_14 46
#define _IRQ_15 47

void irq_init();

void irq_register();

void irq_enable(int i);

void irq_disable(int i);

#endif
