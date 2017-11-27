//
// string.c - implementation of string.h
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

#include <stddef.h>
#include "string.h"

static inline size_t min(size_t a, size_t b)
{
    if(a < b)
        return a;
    return b;
}

void *memcpy(void *restrict s1, const void *restrict s2, size_t n)
{
    unsigned char *buf1 = (unsigned char *) s1;
    const unsigned char *buf2 = (const unsigned char *) s2;
    for(size_t i = 0; i < n; i++)
        buf1[i] = buf2[i];
    return s1;
}

void *memmove(void *s1, const void *s2, size_t n)
{
    unsigned char *buf1 = (unsigned char *) s1;
    const unsigned char *buf2 = (unsigned char *) s2;
    if(buf1 < buf2)
        for(size_t i = 0; i < n; i++)
            buf1[i] = buf2[i];
    else if(buf1 > buf2)
        for(size_t i = n; i > 0; i--)
            buf1[i - 1] = buf2[i - 1];
    return s1;
}

void *memset(void *s, int c, size_t n)
{
    unsigned char *buf = (unsigned char *) s;
    for(size_t i = 0; i < n; i++)
        buf[i] = (unsigned char)c;
    return s;
}

