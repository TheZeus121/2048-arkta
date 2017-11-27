//
// string.h - string.h as defined by POSIX.1-2008
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

#ifndef _STRING_H
#define _STRING_H

#include <stddef.h>
//#include <locale.h>
//http://pubs.opengroup.org/onlinepubs/9699919799/functions/strcoll.html

// The memcpy() function shall copy n bytes from the object pointed to by s2
// into the object pointed to by s1. If copying takes place between objects that
// overlap, the behavior is undefined.
void *memcpy(void *restrict s1, const void *restrict s2, size_t n);

// The memmove() function shall copy n bytes from the object pointed to by s2
// into the object pointed to by s1. Copying takes place as if the n bytes from
// the object pointed to by s2 are first copied into a temporary array of n
// bytes that does not overlap the objects pointed to by s1 and s2, and then the
// n bytes from the temporary array are copied into the object pointed to by s1.
void *memmove(void *s1, const void *s2, size_t n);

// The memset() function shall copy c (converted to an unsigned char) into each
// of the first n bytes of the object pointed to by s.
void *memset(void *s, int c, size_t n);

#endif
