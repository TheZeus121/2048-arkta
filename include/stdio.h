//
// stdio.h as defined by POSIX.1-2008
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

#ifndef _STDIO_H
#define _STDIO_H

// THIS IS VERY INCOMPLETE
// ONLY IS SUPPORTING printf for kernel rn

// should be defined by gcc freestanding
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __kernel__

void _text_drawfield(int[4][4], bool, bool, uint64_t, uint64_t);
void _text_init();
void _text_switchstyle();

#endif

#define EOF 0x7E

// Temporary
#define stdout ((FILE *) 0)
#define stderr ((FILE *) 0)

//TODO
typedef struct FILE_struct
{
    
} FILE;

int fprintf(FILE *restrict, const char *restrict, ...);
int fputc(int c, FILE* stream);
int fputs(const char *restrict s, FILE *restrict stream);
int printf(const char *restrict format, ...);
int putc(int c, FILE* stream);
int putchar(int c);
int puts(const char * s);
int vfprintf(FILE *restrict stream, const char *restrict format, va_list ap);
int vprintf(const char *restrict format, va_list ap);

#endif
