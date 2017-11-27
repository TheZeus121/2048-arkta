//
// vfprintf.c - implementation of vfprintf() from stdio.h
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

#include <stdbool.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include "stdio.h"

int vfprintf(FILE *restrict stream, const char *restrict format, va_list ap)
{
    for(size_t i = 0; format[i] != 0; i++)
    {
        if(format[i] != '%')
        {
            fputc(format[i], stream);
        }
        else
        {
            i++;
            bool ischar = false;
            bool isshort = false;
            bool islong = false;
            bool islonglong = false;
            bool isintmax = false;
            bool issize = false;
            bool isptrdiff = false;
            
            bool isunsigned = false;
            bool capitals = false;
            
            size_t base = 0;
            
            bool aschar = false;
            bool asstr = false;
            bool asptr = false;
            bool nothing = false;
            
            // conversion specifiers
            switch(format[i])
            {
                case 'd':
                case 'i':       // signed decimal integer
                    base = 10;
                    break;
                case 'o':
                    isunsigned = true;
                    base = 8;
                    break;
                case 'u':
                    isunsigned = true;
                    base = 10;
                    break;
                case 'p':
                    asptr = true;
                    /* FALLTHROUGH */
                case 'X':
                    capitals = true;
                    /* FALLTHROUGH */
                case 'x':
                    isunsigned = true;
                    base = 16;
                    break;
                case 'c':
                    aschar = true;
                    break;
                case 's':
                    asstr = true;
                    break;
                case '%':
                    nothing = true;
                    break;
                case 'M':   // NOT STANDARD
                    base = 10;
                    isunsigned = true;
                    isintmax = true;
                    break;
                default:
                    return EOF;
            }
            
            if(aschar)
            {
                char c = (char) va_arg(ap, int);
                if(fputc(c, stream) != c)
                    return EOF;
            }
            else if(asstr)
            {
                char* s2 = va_arg(ap, char *);
                if(fputs(s2, stream) == EOF)
                    return EOF;
            }
            
            if(asptr)
            {
                fputs("PTR:", stream);
            }
            
            if(nothing)
            {
                if(fputc('%', stream) != '%')
                    return EOF;
            }
            else if(base != 0)
            {
                char* digits = "0123456789abcdef";
                if(capitals)
                    digits = "0123456789ABCDEF";
                if(isunsigned)
                {
                    uintmax_t value;
                    if(ischar)
                        value = (uintmax_t)(unsigned char)va_arg(ap, int);
                    else if(isshort)
                        value = (uintmax_t)(unsigned short)va_arg(ap, int);
                    else if(islong)
                        value = (uintmax_t)va_arg(ap, unsigned long);
                    else if(islonglong)
                        value = (uintmax_t)va_arg(ap, unsigned long long);
                    else if(isintmax)
                        value = (uintmax_t)va_arg(ap, uintmax_t);
                    else if(issize)
                        value = (uintmax_t)va_arg(ap, size_t);
                    else if(isptrdiff)
                        value = (uintmax_t)va_arg(ap, ptrdiff_t);
                    else
                        value = (uintmax_t)va_arg(ap, unsigned int);
                    if(value == 0)
                    {
                        if(fputc('0', stream) != '0')
                            return EOF;
                    }
                    else
                    {
                        char output[50];
                        output[49] = 0;
                        char* loc = &output[48];
                        for(;value != 0; loc--, value /= base)
                            *loc = digits[value % base];
                        loc++;
                        if(fputs(loc, stream) == EOF)
                            return EOF;
                    }
                }
                else
                {
                    intmax_t value;
                    if(ischar)
                        value = (intmax_t)(char)va_arg(ap, int);
                    else if(isshort)
                        value = (intmax_t)(short)va_arg(ap, int);
                    else if(islong)
                        value = (intmax_t)va_arg(ap, long);
                    else if(islonglong)
                        value = (intmax_t)va_arg(ap, long long);
                    else if(isintmax)
                        value = (intmax_t)va_arg(ap, intmax_t);
                    else if(issize)
                        value = (intmax_t)va_arg(ap, size_t);
                    else if(isptrdiff)
                        value = (intmax_t)va_arg(ap, ptrdiff_t);
                    else
                        value = (intmax_t)va_arg(ap, int);
                    if(value < 0)
                    {
                        if(fputc('-', stream) != '-')
                            return EOF;
                        value = -value;
                    }
                    if(value == 0)
                    {
                        if(fputc('0', stream) != '0')
                            return EOF;
                    }
                    else
                    {
                        char output[50];
                        output[49] = 0;
                        char *loc = &output[48];
                        for(;value != 0; loc--, value /= base)
                            *loc = digits[value % base];
                        loc++;
                        if(fputs(loc, stream) == EOF)
                            return EOF;
                    }
                }
            }
        }
    }
    return 69;
}
