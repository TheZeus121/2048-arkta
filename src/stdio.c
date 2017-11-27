//
// stdio.c - implementation of stdio.h
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
#include <stddef.h>
#include <stdint.h>
#include "stdio.h"

#ifdef __kernel__

#include "string.h"

#define _VGA_WIDTH  80
#define _VGA_HEIGHT 25

#define _VGA_BLACK          0
#define _VGA_BLUE           1
#define _VGA_GREEN          2
#define _VGA_CYAN           3
#define _VGA_RED            4
#define _VGA_MAGENTA        5
#define _VGA_BROWN          6
#define _VGA_LIGHTGRAY      7
#define _VGA_DARKGRAY       8
#define _VGA_LIGHTBLUE      9
#define _VGA_LIGHTGREEN     10
#define _VGA_LIGHTCYAN      11
#define _VGA_LIGHTRED       12
#define _VGA_LIGHTMAGENTA   13
#define _VGA_LIGHTBROWN     14
#define _VGA_WHITE          15

void outb(uint16_t port, uint8_t value);

// 0x03D0 - 0x3DF -- CGA (Color Graphics Adapter)
#define _VGA_SELECT_REGISTER        0x03D4
#define _VGA_REGISTER_CURSORSTART   0x0A
#define _VGA_DATA_CURSOROFF         (1<<5)
#define _VGA_REGISTER_CURSOREND     0x0B
#define _VGA_REGISTER_CURSORLOCHIGH 0x0E
#define _VGA_REGISTER_CURSORLOCLOW  0x0F
#define _VGA_DATA_REGISTER          0x03D5

static uint16_t cursor_y;
static uint16_t cursor_x;

#define attrib (_VGA_BLACK << 4) | (_VGA_LIGHTGRAY)

static uint16_t *video_mem;

static uint16_t blank;

static bool alternate;

#define MAP_WIDTH 21*2
#define MAP_HEIGHT 9

// from code page 437

// double up left corner - 201
// double up right corner - 187
// double down left corner - 200
// double down right corner - 188

// double horizontal - 205
// double vertical - 186

// horizontal to down - 209
// horizontal to up - 207
// vertical to right - 199
// vertical to left - 182

// single vertical - 179
// single horizontal - 196

// cross - 197

char main_map[MAP_HEIGHT * MAP_WIDTH] = {
    201, attrib, 205, attrib, 205, attrib, 205, attrib, 205, attrib, 209, attrib, 205, attrib, 205, attrib, 205, attrib, 205, attrib, 209, attrib, 205, attrib, 205, attrib, 205, attrib, 205, attrib, 209, attrib, 205, attrib, 205, attrib, 205, attrib, 205, attrib, 187, attrib,
    186, attrib, 32, attrib, 32, attrib, 32, attrib, 32, attrib, 179, attrib, 32, attrib, 32, attrib, 32, attrib, 32, attrib, 179, attrib, 32, attrib, 32, attrib, 32, attrib, 32, attrib, 179, attrib, 32, attrib, 32, attrib, 32, attrib, 32, attrib, 186, attrib,
    199, attrib, 196, attrib, 196, attrib, 196, attrib, 196, attrib, 197, attrib, 196, attrib, 196, attrib, 196, attrib, 196, attrib, 197, attrib, 196, attrib, 196, attrib, 196, attrib, 196, attrib, 197, attrib, 196, attrib, 196, attrib, 196, attrib, 196, attrib, 182, attrib,
    186, attrib, 32, attrib, 32, attrib, 32, attrib, 32, attrib, 179, attrib, 32, attrib, 32, attrib, 32, attrib, 32, attrib, 179, attrib, 32, attrib, 32, attrib, 32, attrib, 32, attrib, 179, attrib, 32, attrib, 32, attrib, 32, attrib, 32, attrib, 186, attrib,
    199, attrib, 196, attrib, 196, attrib, 196, attrib, 196, attrib, 197, attrib, 196, attrib, 196, attrib, 196, attrib, 196, attrib, 197, attrib, 196, attrib, 196, attrib, 196, attrib, 196, attrib, 197, attrib, 196, attrib, 196, attrib, 196, attrib, 196, attrib, 182, attrib,
    186, attrib, 32, attrib, 32, attrib, 32, attrib, 32, attrib, 179, attrib, 32, attrib, 32, attrib, 32, attrib, 32, attrib, 179, attrib, 32, attrib, 32, attrib, 32, attrib, 32, attrib, 179, attrib, 32, attrib, 32, attrib, 32, attrib, 32, attrib, 186, attrib,
    199, attrib, 196, attrib, 196, attrib, 196, attrib, 196, attrib, 197, attrib, 196, attrib, 196, attrib, 196, attrib, 196, attrib, 197, attrib, 196, attrib, 196, attrib, 196, attrib, 196, attrib, 197, attrib, 196, attrib, 196, attrib, 196, attrib, 196, attrib, 182, attrib,
    186, attrib, 32, attrib, 32, attrib, 32, attrib, 32, attrib, 179, attrib, 32, attrib, 32, attrib, 32, attrib, 32, attrib, 179, attrib, 32, attrib, 32, attrib, 32, attrib, 32, attrib, 179, attrib, 32, attrib, 32, attrib, 32, attrib, 32, attrib, 186, attrib,
    200, attrib, 205, attrib, 205, attrib, 205, attrib, 205, attrib, 207, attrib, 205, attrib, 205, attrib, 205, attrib, 205, attrib, 207, attrib, 205, attrib, 205, attrib, 205, attrib, 205, attrib, 207, attrib, 205, attrib, 205, attrib, 205, attrib, 205, attrib, 188, attrib
};

char alt_map[MAP_HEIGHT * MAP_WIDTH] = {
    '+', attrib, '-', attrib, '-', attrib, '-', attrib, '-', attrib, '+', attrib, '-', attrib, '-', attrib, '-', attrib, '-', attrib, '+', attrib, '-', attrib, '-', attrib, '-', attrib, '-', attrib, '+', attrib, '-', attrib, '-', attrib, '-', attrib, '-', attrib, '+', attrib,
    '|', attrib, ' ', attrib, ' ', attrib, ' ', attrib, ' ', attrib, '|', attrib, ' ', attrib, ' ', attrib, ' ', attrib, ' ', attrib, '|', attrib, ' ', attrib, ' ', attrib, ' ', attrib, ' ', attrib, '|', attrib, ' ', attrib, ' ', attrib, ' ', attrib, ' ', attrib, '|', attrib,
    '+', attrib, '-', attrib, '-', attrib, '-', attrib, '-', attrib, '+', attrib, '-', attrib, '-', attrib, '-', attrib, '-', attrib, '+', attrib, '-', attrib, '-', attrib, '-', attrib, '-', attrib, '+', attrib, '-', attrib, '-', attrib, '-', attrib, '-', attrib, '+', attrib,
    '|', attrib, ' ', attrib, ' ', attrib, ' ', attrib, ' ', attrib, '|', attrib, ' ', attrib, ' ', attrib, ' ', attrib, ' ', attrib, '|', attrib, ' ', attrib, ' ', attrib, ' ', attrib, ' ', attrib, '|', attrib, ' ', attrib, ' ', attrib, ' ', attrib, ' ', attrib, '|', attrib,
    '+', attrib, '-', attrib, '-', attrib, '-', attrib, '-', attrib, '+', attrib, '-', attrib, '-', attrib, '-', attrib, '-', attrib, '+', attrib, '-', attrib, '-', attrib, '-', attrib, '-', attrib, '+', attrib, '-', attrib, '-', attrib, '-', attrib, '-', attrib, '+', attrib,
    '|', attrib, ' ', attrib, ' ', attrib, ' ', attrib, ' ', attrib, '|', attrib, ' ', attrib, ' ', attrib, ' ', attrib, ' ', attrib, '|', attrib, ' ', attrib, ' ', attrib, ' ', attrib, ' ', attrib, '|', attrib, ' ', attrib, ' ', attrib, ' ', attrib, ' ', attrib, '|', attrib,
    '+', attrib, '-', attrib, '-', attrib, '-', attrib, '-', attrib, '+', attrib, '-', attrib, '-', attrib, '-', attrib, '-', attrib, '+', attrib, '-', attrib, '-', attrib, '-', attrib, '-', attrib, '+', attrib, '-', attrib, '-', attrib, '-', attrib, '-', attrib, '+', attrib,
    '|', attrib, ' ', attrib, ' ', attrib, ' ', attrib, ' ', attrib, '|', attrib, ' ', attrib, ' ', attrib, ' ', attrib, ' ', attrib, '|', attrib, ' ', attrib, ' ', attrib, ' ', attrib, ' ', attrib, '|', attrib, ' ', attrib, ' ', attrib, ' ', attrib, ' ', attrib, '|', attrib,
    '+', attrib, '-', attrib, '-', attrib, '-', attrib, '-', attrib, '+', attrib, '-', attrib, '-', attrib, '-', attrib, '-', attrib, '+', attrib, '-', attrib, '-', attrib, '-', attrib, '-', attrib, '+', attrib, '-', attrib, '-', attrib, '-', attrib, '-', attrib, '+', attrib
};

#define col2    (_VGA_BLACK   << 4) | _VGA_LIGHTGRAY
#define col4    (_VGA_BLACK   << 4) | _VGA_LIGHTBLUE
#define col8    (_VGA_BLACK   << 4) | _VGA_LIGHTGREEN
#define col16   (_VGA_BLACK   << 4) | _VGA_LIGHTCYAN
#define col32   (_VGA_BLACK   << 4) | _VGA_LIGHTRED
#define col64   (_VGA_BLACK   << 4) | _VGA_LIGHTMAGENTA
#define col128  (_VGA_BLACK   << 4) | _VGA_LIGHTBROWN
#define col256  (_VGA_BLUE    << 4) | _VGA_WHITE
#define col512  (_VGA_GREEN   << 4) | _VGA_WHITE
#define col1024 (_VGA_CYAN    << 4) | _VGA_WHITE
#define col2048 (_VGA_RED     << 4) | _VGA_WHITE
#define col4096 (_VGA_MAGENTA << 4) | _VGA_WHITE
#define col8192 (_VGA_BROWN   << 4) | _VGA_WHITE

char text[13][8] = {
    {' ', col2,    ' ', col2,    '2', col2,    ' ', col2},      // 1
    {' ', col4,    ' ', col4,    '4', col4,    ' ', col4},      // 2
    {' ', col8,    ' ', col8,    '8', col8,    ' ', col8},      // 3
    {' ', col16,   '1', col16,   '6', col16,   ' ', col16},     // 4
    {' ', col32,   '3', col32,   '2', col32,   ' ', col32},     // 5
    {' ', col64,   '6', col64,   '4', col64,   ' ', col64},     // 6
    {' ', col128,  '1', col128,  '2', col128,  '8', col128},    // 7
    {' ', attrib,  '2', col256,  '5', col256,  '6', col256},    // 8
    {' ', attrib,  '5', col512,  '1', col512,  '2', col512},    // 9
    {'1', col1024, '0', col1024, '2', col1024, '4', col1024},   // 10
    {'2', col2048, '0', col2048, '4', col2048, '8', col2048},   // 11
    {'4', col4096, '0', col4096, '9', col4096, '6', col4096},   // 12
    {'8', col8192, '1', col8192, '9', col8192, '2', col8192}    // 13
}; // I won't go further than this so sorry if someone's game crashes 

static inline uint16_t _entry(uint8_t c, uint8_t attr)
{
    return ((uint16_t) c) | (((uint16_t) attr) << 8);
}

static void _clear();
static void _move_cur();
static void _scroll();

void _text_init()
{
    outb(_VGA_SELECT_REGISTER, _VGA_REGISTER_CURSORSTART);
    outb(_VGA_DATA_REGISTER, 0);
    outb(_VGA_SELECT_REGISTER, _VGA_REGISTER_CURSOREND);
    outb(_VGA_DATA_REGISTER, 15);
    video_mem = (uint16_t *) 0xB8000;
    blank = _entry(' ', attrib);
    alternate = false;
    _clear();
}

void _text_drawfield(int data[4][4], bool lost, bool won, uint64_t score, 
                     uint64_t highscore)
{
    _clear();
    char* map;
    if(alternate)
        map = (char*) alt_map;
    else
        map = (char*) main_map;
    char* buf = (char*) video_mem;
    for (int i = 0; i < MAP_HEIGHT; i++)
    {
        memcpy(buf, map + i * MAP_WIDTH, MAP_WIDTH);
        buf = (char *) ((uint32_t) buf + _VGA_WIDTH * 2);
    }
    
    buf = (char *) video_mem + 2 + _VGA_WIDTH * 2;
    for (int x = 0; x < 4; x++)
    {
        for (int y = 0; y < 4; y++)
        {
            if (data[x][y] != 0)
            {
                memcpy(buf, text + data[x][y] - 1, 8);
            }
            buf += 10;
        }
        buf += _VGA_WIDTH * 4 - 40;
    }
    
    cursor_x = MAP_WIDTH / 2 + 2;       // COL 44
    cursor_y = 1;
    // print score
    printf("Score: %M", score);
    
    cursor_x = MAP_WIDTH / 2 + 2;
    cursor_y = 3;
    printf("Highscore: %M", highscore);
    
    cursor_x = MAP_WIDTH / 2 + 4;
    cursor_y = 4;
    printf("!Highscores are not stored!");
    
    if (won)
    {
        cursor_x = MAP_WIDTH / 2 + 2;
        cursor_y = 6;
        printf("You won the game! Congratulations! :D");
    }
    else if (lost)
    {
        cursor_x = MAP_WIDTH / 2 + 2;
        cursor_y = 6;
        printf("You lost the game! Better luck next time! :(");
    }
    
    cursor_x = 0;
    cursor_y = MAP_HEIGHT + 1;
    _move_cur();
    printf("Welcome to 2048/Arkta!\n"); // LINE 11
    cursor_y++;                         // LINE 12
    printf("INSTRUCTIONS:\n");          // LINE 13
                                        // LINE 14
    printf("Use arrow keys or WASD to move the numbers to get 2048!\n");
    cursor_y++;                         // LINE 15
    printf("Additional keys:\n");       // LINE 16
    printf("b B - switch between style of borders \n\
        (fancy and basic, fancy may not be supported)\n");
                                        // LINE 17
                                        // LINE 18
    printf("r R - restart the game\n"); // LINE 19
    cursor_y++;                         // LINE 20
                                        // LINE 21
    printf("To exit the game just press the power on/off button on your PC\n");
    cursor_y++;                         // LINE 22
    cursor_y++;                         // LINE 23
    printf("Have fun! :D\n");           // LINE 24
}

void _text_switchstyle()
{
    alternate = !alternate;
}

static void _clear()
{
    for(size_t i = 0; i < _VGA_WIDTH * _VGA_HEIGHT; i++)
        video_mem[i] = blank;
    cursor_x = 0;
    cursor_y = 0;
    _move_cur();
}

static void _move_cur()
{
    uint16_t loc = cursor_y * 80 + cursor_x;
    outb(_VGA_SELECT_REGISTER, _VGA_REGISTER_CURSORLOCHIGH);
    outb(_VGA_DATA_REGISTER, loc >> 8);
    outb(_VGA_SELECT_REGISTER, _VGA_REGISTER_CURSORLOCLOW);
    outb(_VGA_DATA_REGISTER, loc & 0xFF);
}

static void _scroll()
{
    if(cursor_y >= 25)
    {
        memmove(video_mem, video_mem + _VGA_WIDTH * (cursor_y - 24),\
                            _VGA_WIDTH * (_VGA_HEIGHT - (cursor_y - 24)) * 2);
        for(size_t i = _VGA_WIDTH * (_VGA_HEIGHT - (cursor_y - 24));\
                                            i < _VGA_WIDTH * _VGA_HEIGHT; i++)
            video_mem[i] = blank;
        cursor_y = 24;
    }
}

// __kernel__
#endif

// TODO errno.h
int fputc(int c, FILE *stream)
{
#ifdef __kernel__
    if(stream != stdout)
        return EOF;
    
    uint16_t entr = _entry((uint8_t) c, attrib);
    uint16_t loc = cursor_y * _VGA_WIDTH + cursor_x;
    
    if(c == '\b' && cursor_x != 0) // Backspace
    {
        cursor_x--;
        video_mem[loc - 1] = blank;
    }
    else if(c == '\t')              // Horizontal Tab
    {
        cursor_x += 8 - (cursor_x % 8);
    }
    else if(c == '\n')              // Line Feed
    {
        cursor_x = 0;
        cursor_y++;
    }
    else if(c == '\v')              // Vertical Tab
    {
        cursor_y += 6 - (cursor_y % 6);
    }
    else if(c == '\f')              // Form Feed
    {
        _clear();
    }
    else if(c == '\r')              // Carriage Return
    {
        cursor_x = 0;
    }
    else if(c >= 0x20 && c != 0x7F) // Is printable (0x7F is DEL)
    {
        video_mem[loc] = entr;
        cursor_x++;
    }
    
    if(cursor_x >= 80)
    {
        cursor_x = 0;
        cursor_y++;
    }
    
    _scroll();
    _move_cur();
    return c;
#else
// Don't actually throw this yet bcs i am autocompiling libc already
//#error Output is not yet implemented for user-mode programs!
    return EOF;
#endif
}

int fprintf(FILE *restrict stream, const char *restrict format, ...)
{
    va_list ap;
    va_start(ap, format);
    int ret = vfprintf(stream, format, ap);
    va_end(ap);
    return ret;
}

int fputs(const char *restrict s, FILE *restrict stream)
{
    for(size_t i = 0; s[i] != 0; i++)
        if(fputc(s[i], stream) != s[i])
            return EOF;
    return 0;
}

int printf(const char *restrict format, ...)
{
    va_list ap;
    va_start(ap, format);
    int ret = vprintf(format, ap);
    va_end(ap);
    return ret;
}

int putc(int c, FILE *stream)
{
    return fputc(c, stream);
}

int putchar(int c)
{
    return fputc(c, stdout);
}

int puts(const char *s)
{
    if(fputs(s, stdout) == EOF)
        return EOF;
    if(fputc('\n', stdout) == '\n')
        return 0;
    return EOF;
}

int vprintf(const char *restrict format, va_list ap)
{
    return vfprintf(stdout, format, ap);
}
