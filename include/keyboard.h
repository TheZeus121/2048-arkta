//
// keyboard.h - PS/2 keyboard driver
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

#ifndef __DRIVER_KEYBOARD_H
#define __DRIVER_KEYBOARD_H

#include <stdbool.h>
#include <stdint.h>

// normal characters

#define KEY_A           1
#define KEY_B           2
#define KEY_C           3
#define KEY_D           4
#define KEY_E           5
#define KEY_F           6
#define KEY_G           7
#define KEY_H           8
#define KEY_I           9
#define KEY_J           10
#define KEY_K           11
#define KEY_L           12
#define KEY_M           13
#define KEY_N           14
#define KEY_O           15
#define KEY_P           16
#define KEY_Q           17
#define KEY_R           18
#define KEY_S           19
#define KEY_T           20
#define KEY_U           21
#define KEY_V           22
#define KEY_W           23
#define KEY_X           24
#define KEY_Y           25
#define KEY_Z           26

#define KEY_LETTER_COUNT 26

#define KEY_0           27
#define KEY_1           28
#define KEY_2           29
#define KEY_3           30
#define KEY_4           31
#define KEY_5           32
#define KEY_6           33
#define KEY_7           34
#define KEY_8           35
#define KEY_9           36
#define KEY_BACKTICK    37
#define KEY_MINUS       38
#define KEY_EQUALS      39
#define KEY_BACKSLASH   40
#define KEY_SPACE       41
#define KEY_TAB         42
#define KEY_ENTER       43
#define KEY_LEFTBRACK   44  /* [ */
#define KEY_RIGHTBRACK  45  /* ] */
#define KEY_SEMICOLON   46
#define KEY_SINGLEQUOT  47  /* ' */
#define KEY_COMMA       48
#define KEY_DOT         49
#define KEY_SLASH       50
#define KEY_BACKSPACE   51

#define KEY_VISIBLE_COUNT 52

// keypad

#define KEY_KPSLASH     52  /* / on keypad */
#define KEY_KPSTAR      53  /* * on keypad */
#define KEY_KPMINUS     54  /* - on keypad */
#define KEY_KPPLUS      55  /* + on keypad */
#define KEY_KPENTER     56  /* Enter on keypad */
#define KEY_KPDOT       57  /* . on keypad */
#define KEY_KP0         58
#define KEY_KP1         59
#define KEY_KP2         60
#define KEY_KP3         61
#define KEY_KP4         62
#define KEY_KP5         63
#define KEY_KP6         64
#define KEY_KP7         65
#define KEY_KP8         66
#define KEY_KP9         67

#define KEY_NUMLOCK_COUNT 68

// locks

#define KEY_CAPSLOCK    68
#define KEY_NUMLOCK     69
#define KEY_SCROLL      70

// modifiers

#define KEY_MODIFIER_START 71

#define KEY_LSHIFT      71
#define KEY_RSHIFT      72
#define KEY_LCTRL       73
#define KEY_RCTRL       74
#define KEY_LGUI        75  /* I believe GUIs are the start buttons */
#define KEY_RGUI        76
#define KEY_LALT        77
#define KEY_RALT        78
#define KEY_APPS        79  /* And this is the menu button */

#define KEY_MODIFIER_END 80

// others

#define KEY_ESC         80
#define KEY_F1          81
#define KEY_F2          82
#define KEY_F3          83
#define KEY_F4          84
#define KEY_F5          85
#define KEY_F6          86
#define KEY_F7          87
#define KEY_F8          88
#define KEY_F9          89
#define KEY_F10         90
#define KEY_F11         91
#define KEY_F12         92
#define KEY_PRNTSCRN    93
#define KEY_PAUSE       94
#define KEY_INSERT      95
#define KEY_HOME        96
#define KEY_PAGEUP      97
#define KEY_DELETE      98
#define KEY_END         99
#define KEY_PAGEDOWN    100
#define KEY_UP          101
#define KEY_LEFT        102
#define KEY_DOWN        103
#define KEY_RIGHT       104

// these are the 'feature keys' found on some older kbs
#define KEY_POWER       105
#define KEY_SLEEP       106
#define KEY_WAKE        107

// I suppose these are the Fn+ keys
#define KEY_NEXTTRACK   108
#define KEY_PREVTRACK   109
#define KEY_STOP        110
#define KEY_PLAY        111
#define KEY_MUTE        112
#define KEY_VOLUMEUP    113
#define KEY_VOLUMEDOWN  114
#define KEY_MEDIASELECT 115
#define KEY_EMAIL       116
#define KEY_CALCULATOR  117
#define KEY_MYCOMPUTER  118
#define KEY_WWWSEARCH   119
#define KEY_WWWHOME     120
#define KEY_WWWBACK     121
#define KEY_WWWFORWARD  122
#define KEY_WWWSTOP     123
#define KEY_WWWREFRESH  124
#define KEY_WWWFAVORITE 125

#define KEY_COUNT       126

#define KB_LEDSTATE     0xED
#define KB_LED_SCROLL   (1<<0)
#define KB_LED_NUM      (1<<1)
#define KB_LED_CAPS     (1<<2)
#define KB_ECHO         0xEE
#define KB_CODESET      0xF0
#define KB_ENABLE       0xF4

#define KB_INT_MAXAGE   100

typedef void (*sendbyte_t)(uint8_t, bool*);
typedef uint8_t (*getscan_t)();
typedef uint8_t (*readbyte_t)(bool*);
typedef void (*wait_t)(void);
typedef void (*waitack_t)(bool*, int*, int);

struct kb_interface_struct
{
    // must be set by the caller of kb_add
    getscan_t   getscan;
    sendbyte_t  sendbyte;
    readbyte_t  readbyte;
    wait_t      wait;
    waitack_t   wait_ack;
    bool        translated;
    
    // will be set by kb_add
    int         mode;
    int         age;
    bool        can_setcode;
};

typedef struct kb_interface_struct kb_interface_t;

void    kb_add(kb_interface_t* interface);
void    kb_start();
void    kb_update();
const   char*   kb_getcharmap();

bool    kb_ispressed(int key);

#endif
