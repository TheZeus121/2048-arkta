//
// keyboard.c - implementation of keyboard.h
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

#include "keyboard.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

kb_interface_t* interfaces[2];
int interface_count = 0;

int buf[2];
int buf_length;

bool pressed[KEY_COUNT];

void kb_add(kb_interface_t* interface)
{
    memset(pressed, 0, KEY_COUNT * sizeof(bool));
    printf("\n");
    bool timeout = false;
    interface->sendbyte(KB_ECHO, &timeout);
    if(timeout)
    {
        printf("Timeout while sending, device possibly removed\n");
        return;
    }
    uint8_t resp = interface->readbyte(&timeout);
    if(timeout)
    {
        printf("Timeout waiting for response from keyboard, "
                                                        "assuming removed\n");
        return;
    }
    if(resp != KB_ECHO)
    {
        printf("Echo command did not echo, received this: 0x%X\n", resp);
        return;
    }
    interface->age = 0;
    
    buf[0] = KB_CODESET;
    buf[1] = 0; // get current codeset
    interface->sendbyte(buf[0], &timeout);
    interface->sendbyte(buf[1], &timeout);
    interface->wait_ack(&timeout, buf, buf_length);
    if(timeout)
    {
        printf("Timeout while getting code set\n");
        printf("Marking as not implemented and as code set 1\n");
        interface->can_setcode = false;
        interface->mode = 1;
    }
    else
    {
        interface->can_setcode = true;
        uint8_t tmp = 0;
        while(tmp != 1 && tmp != 2 && tmp != 3)
            tmp = interface->readbyte(&timeout);
        interface->mode = tmp;
    }
    
    if(interface->translated)
    {
        interface->mode = 1;
    }
    else if(interface->can_setcode && interface->mode != 1 && 
                                                        interface->mode != 2)
    {
        buf[0] = KB_CODESET;
        buf[1] = 1; // set to 1
        interface->sendbyte(buf[0], &timeout);
        interface->sendbyte(buf[1], &timeout);
        interface->wait_ack(&timeout, buf, buf_length);
        if(!timeout)
        {
            buf[1] = 0;
            interface->sendbyte(buf[0], &timeout);
            interface->sendbyte(buf[1], &timeout);
            interface->wait_ack(&timeout, buf, buf_length);
            if(!timeout)
            {
                uint8_t tmp = 0;
                while(tmp != 1 && tmp != 2 && tmp != 3)
                    tmp = interface->readbyte(&timeout);
                interface->mode = tmp;
            }
        }
    }
    
    // if the kb didn't support code page 1, maybe it will support 2
    if(interface->can_setcode && interface->mode != 1 && interface->mode != 2)
    {
        buf[0] = KB_CODESET;
        buf[1] = 2;
        interface->sendbyte(buf[0], &timeout);
        interface->sendbyte(buf[1], &timeout);
        interface->wait_ack(&timeout, buf, buf_length);
        if(!timeout)
        {
            buf[1] = 0;
            interface->sendbyte(buf[0], &timeout);
            interface->sendbyte(buf[1], &timeout);
            interface->wait_ack(&timeout, buf, buf_length);
            if(!timeout)
            {
                uint8_t tmp = 0;
                while(tmp != 1 && tmp != 2 && tmp != 3)
                    tmp = interface->readbyte(&timeout);
                interface->mode = tmp;
            }
        }
    }
    
    printf("Current code page: 0x%X\n", interface->mode);
    
    interfaces[interface_count] = interface;
    interface_count++;
}

bool kb_ispressed(int key)
{
    if(pressed[key])
    {
        pressed[key] = false;
        return true;
    }
    return false;
}

void kb_start()
{
    bool timeout = false;
    for(int i = 0; i < interface_count; i++)
    {
        interfaces[i]->sendbyte(KB_ENABLE, &timeout);
        buf[0] = KB_ENABLE;
        buf_length = 1;
        interfaces[i]->wait_ack(&timeout, buf, buf_length);
    }
}

void kb_update()
{
    uint8_t code = 1;
    for(int i = 0; i < interface_count; i++)
    {
            code = interfaces[i]->getscan();
            pressed[KEY_PAUSE] = false;
            if(interfaces[i]->mode == 1)
            {
                bool value = true;
                if(code > 0x80 && code != 0xE0 && code != 0xE1)
                {
                    value = false;
                    code -= 0x80;
                }
                switch(code)
                {
                    case 0x11:
                        pressed[KEY_UP] = value; // W
                        break;
                    case 0x13:
                        pressed[KEY_R] = value;
                        break;
                    case 0x1E:
                        pressed[KEY_LEFT] = value; // A
                        break;
                    case 0x1F:
                        pressed[KEY_DOWN] = value; // S
                        break;
                    case 0x20:
                        pressed[KEY_RIGHT] = value; // D
                        break;
                    case 0x30:
                        pressed[KEY_B] = value;
                        break;
                    case 0x48:
                        pressed[KEY_UP] = value; // KP8
                        break;
                    case 0x4B:
                        pressed[KEY_LEFT] = value; // KP4
                        break;
                    case 0x4D:
                        pressed[KEY_RIGHT] = value; // KP6
                        break;
                    case 0x50:
                        pressed[KEY_DOWN] = value; // KP2
                        break;
                    
                    case 0xE0:
                        code = interfaces[i]->getscan();
                        value = true;
                        if(code > 0x80 && code != 0xB7)
                        {
                            value = false;
                            code -= 0x80;
                        }
                        switch(code)
                        {
                            case 0x48:
                                pressed[KEY_UP] = value;
                                break;
                            case 0x4B:
                                pressed[KEY_LEFT] = value;
                                break;
                            case 0x4D:
                                pressed[KEY_RIGHT] = value;
                                break;
                            case 0x50:
                                pressed[KEY_DOWN] = value;
                                break;
                            
                            case 0x2A:
                                code = interfaces[i]->getscan();
                                if(code != 0xE0)
                                    break;
                                code = interfaces[i]->getscan();
                                if(code != 0x37)
                                    break;
                                break;
                            case 0xB7:
                                code = interfaces[i]->getscan();
                                if(code != 0xE0)
                                    break;
                                code = interfaces[i]->getscan();
                                if(code != 0xAA)
                                    break;
                                break;
                        }
                        break;
                    
                    case 0xE1:
                        code = interfaces[i]->getscan();
                        if(code != 0x1D)
                            break;
                        code = interfaces[i]->getscan();
                        if(code != 0x45)
                            break;
                        code = interfaces[i]->getscan();
                        if(code != 0xE1)
                            break;
                        code = interfaces[i]->getscan();
                        if(code != 0x9D)
                            break;
                        code = interfaces[i]->getscan();
                        if(code != 0xC5)
                            break;
                        break;
                }
            }
            else if(interfaces[i]->mode == 2)
            {
                bool value = true;
                if(code == 0xF0)
                {
                    value = false;
                    code = interfaces[i]->getscan();
                }
                switch(code)
                {
                    case 0x1B:
                        pressed[KEY_DOWN] = value; // S
                        break;
                    case 0x1C:
                        pressed[KEY_LEFT] = value; // A
                        break;
                    case 0x1D:
                        pressed[KEY_UP] = value; // W
                        break;
                    case 0x23:
                        pressed[KEY_RIGHT] = value; // D
                        break;
                    case 0x2D:
                        pressed[KEY_R] = value;
                        break;
                    case 0x32:
                        pressed[KEY_B] = value;
                        break;
                    case 0x6B:
                        pressed[KEY_LEFT] = value; // KP4
                        break;
                    case 0x72:
                        pressed[KEY_DOWN] = value; // KP2
                        break;
                    case 0x74:
                        pressed[KEY_RIGHT] = value; // KP6
                        break;
                    case 0x75:
                        pressed[KEY_UP] = value; // KP8
                        break;
                    
                    case 0xE0:
                        code = interfaces[i]->getscan();
                        value = true;
                        if(code == 0xF0)
                        {
                            value = false;
                            code = interfaces[i]->getscan();
                        }
                        switch(code)
                        {
                            case 0x6B:
                                pressed[KEY_LEFT] = value;
                                break;
                            case 0x72:
                                pressed[KEY_DOWN] = value;
                                break;
                            case 0x74:
                                pressed[KEY_RIGHT] = value;
                                break;
                            case 0x75:
                                pressed[KEY_UP] = value;
                                break;
                            
                            case 0x12:
                                code = interfaces[i]->getscan();
                                if(code != 0xE0)
                                    break;
                                code = interfaces[i]->getscan();
                                if(code != 0x7C)
                                    break;
                                break;
                            
                            case 0x7C:
                                if(value) // 2nd code is actually 0xF0 not 0x7C
                                    break;
                                code = interfaces[i]->getscan();
                                if(code != 0xE0)
                                    break;
                                code = interfaces[i]->getscan();
                                if(code != 0xF0)
                                    break;
                                code = interfaces[i]->getscan();
                                if(code != 0x12)
                                    break;
                                break;
                        }
                        break;
                    
                    case 0xE1:
                        code = interfaces[i]->getscan();
                        if(code != 0x14)
                            break;
                        code = interfaces[i]->getscan();
                        if(code != 0x77)
                            break;
                        code = interfaces[i]->getscan();
                        if(code != 0xE1)
                            break;
                        code = interfaces[i]->getscan();
                        if(code != 0xF0)
                            break;
                        code = interfaces[i]->getscan();
                        if(code != 0x14)
                            break;
                        code = interfaces[i]->getscan();
                        if(code != 0xF0)
                            break;
                        code = interfaces[i]->getscan();
                        if(code != 0x77)
                            break;
                        break;
                }
            }
            else if(interfaces[i]->mode == 3)
            {
                bool value = true;
                if(code == 0xF0)
                {
                    value = false;
                    code = interfaces[i]->getscan();
                }
                switch(code)
                {
                    case 0x1B:
                        pressed[KEY_DOWN] = value; // S
                        break;
                    case 0x1C:
                        pressed[KEY_LEFT] = value; // A
                        break;
                    case 0x1D:
                        pressed[KEY_UP] = value; // W
                        break;
                    case 0x23:
                        pressed[KEY_RIGHT] = value; // D
                        break;
                    case 0x2D:
                        pressed[KEY_R] = value;
                        break;
                    case 0x32:
                        pressed[KEY_B] = value;
                        break;
                    case 0x60:
                        pressed[KEY_DOWN] = value;
                        break;
                    case 0x61:
                        pressed[KEY_LEFT] = value;
                        break;
                    case 0x63:
                        pressed[KEY_UP] = value;
                        break;
                    case 0x6A:
                        pressed[KEY_RIGHT] = value;
                        break;
                    case 0x6B:
                        pressed[KEY_LEFT] = value; // KP4
                        break;
                    case 0x72:
                        pressed[KEY_DOWN] = value; // KP2
                        break;
                    case 0x74:
                        pressed[KEY_RIGHT] = value; // KP6
                        break;
                    case 0x75:
                        pressed[KEY_UP] = value; // KP8
                        break;
                }
            }
    }
}
