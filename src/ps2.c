//
// ps2.c - implementation of ps2.h
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

#include "irq.h"
#include "ps2.h"
#include "keyboard.h"
#include "ports.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

const int timeout = 100000;

static kb_interface_t f, s;

static device_t first;
static device_t second;

static bool inited;
static bool timeouted;
static bool has_second;
static uint8_t config;

static uint8_t buf1[256];
static uint8_t buf2[256];
static uint8_t buf1_len;
static uint8_t buf2_len;

// Have this in a cycle in case the controller buffer is larger than 1-byte
static void flush()
{
    uint8_t status;
    int time = 0;
    do
    {
        if(time > timeout)
        {
            timeouted = true;
            return;
        }
        time++;
        status = inb(_PS2_REGISTER_PORT);
        inb(_PS2_DATA_PORT);
    } while(status & _PS2_STATUS_OUT_BUFFER);
}

static int get_type(device_t device)
{
    if(device.code.length == 0)
    {
        printf("Ancient AT keyboard with translation enabled\n");
        return _PS2_TYPE_TRANSLATED_AT_KB;
    }
    
    if(device.code.length == 1)
    {
        if(device.code.code[0] == 0xAB)
        {
            printf("Unknown device, assuming keyboard\n");
            printf("Code: 0xAB\n");
            return _PS2_TYPE_UNKNOWN_KB;
        }
        
        printf("Unknown device, not using\n");
        printf("Code: 0x%X\n", device.code.code[0]);
        return _PS2_TYPE_UNKNOWN;
    }
    
    if(device.code.length == 2)
    {
        if(device.code.code[0] != 0xAB)
        {
            printf("Unknown device, not using\n");
            printf("Code: 0x%X 0x%X\n", device.code.code[0], device.code.code[1]);
            return _PS2_TYPE_UNKNOWN;
        }
        
        if(device.code.code[1] == 0x41 || device.code.code[1] == 0xC1)
        {
            printf("MF2 keyboard with translation enabled\n");
            return _PS2_TYPE_TRANSLATED_MF2_KB;
        }
        
        if(device.code.code[1] == 0x83)
        {
            printf("MF2 keyboard\n");
            return _PS2_TYPE_MF2_KB;
        }
        
        printf("Unkown device, assuming keyboard\n");
        printf("Code: 0xAB 0x%X\n", device.code.code[1]);
        return _PS2_TYPE_UNKNOWN_KB;
    }
    
    printf("Error reading the device ID, length=%i\n", device.code.length);
    return _PS2_TYPE_UNKNOWN;
}

static uint8_t readb()  // must have these checks only for _PS2_DATA_PORT
{
    uint8_t status;
    int time = 0;
    do
    {
        if(time > timeout)
        {
            timeouted = true;
            return 0;
        }
        time++;
        io_wait();
        status = inb(_PS2_REGISTER_PORT);
    } while(!(status & _PS2_STATUS_OUT_BUFFER));
    return inb(_PS2_DATA_PORT);
}

static void wait()
{
    io_wait();
    io_wait();
    io_wait();
    io_wait();
    io_wait();
    io_wait();
    io_wait();
    io_wait();
    io_wait();
    io_wait();
}

static uint8_t read(bool* has_timeout)
{
    timeouted = false;
    uint8_t ret = readb();
    *has_timeout = timeouted;
    return ret;
}

static uint8_t read_f(bool* has_timeout)
{
    while(buf1_len == 0)
    {
        asm volatile("hlt" : :);
        wait();
    }
    *has_timeout = false;
    buf1_len--;
    return buf1[buf1_len];
}

static uint8_t read_s(bool* has_timeout)
{
    while(buf2_len == 0)
    {
        asm volatile("hlt" : :);
        wait();
    }
    *has_timeout = false;
    buf2_len--;
    return buf2[buf2_len];
}

static void writeb(uint32_t port, uint8_t data)
{
    uint8_t status;
    int time = 0;
    do
    {
        if(time > timeout)
        {
            timeouted = true;
            return;
        }
        time++;
        io_wait();
        status = inb(_PS2_REGISTER_PORT);
    } while(status & _PS2_STATUS_IN_BUFFER);
    outb(port, data);
}

static void send_first(uint8_t data, bool* has_timeout)
{
    timeouted = false;
    writeb(_PS2_DATA_PORT, data);
    *has_timeout = timeouted;
    wait();
}

static void send_second(uint8_t data, bool* has_timeout)
{
    timeouted = false;
    writeb(_PS2_REGISTER_PORT, _PS2_SEND_SECOND);
    writeb(_PS2_DATA_PORT, data);
    *has_timeout = timeouted;
    wait();
}

static void wait_ack(bool f, uint8_t resend)
{
    uint8_t tmp = readb();
    int time = 0;
    while(tmp != _PS2_DEVICE_ACK)
    {
        if(time > timeout)
        {
            timeouted = true;
            return;
        }
        time++;
        if(tmp == _PS2_DEVICE_RESEND)
        {
            if(!f)
                writeb(_PS2_REGISTER_PORT, _PS2_SEND_SECOND);
            writeb(_PS2_DATA_PORT, resend);
        }
        tmp = readb();
    }
}

static void wait_ack_f(bool* has_timeout, int* data, int data_size)
{
    uint8_t tmp = readb();
    int time = 0;
    *has_timeout = false;
    while(tmp != _PS2_DEVICE_ACK)
    {
        if(time > timeout)
        {
            *has_timeout = true;
            return;
        }
        time++;
        if(tmp == _PS2_DEVICE_RESEND)
        {
            for(int i = 0; i < data_size; i++)
            {
                writeb(_PS2_DATA_PORT, data[i]);
                wait();
            }
        }
        tmp = readb();
    }
}

static void wait_ack_s(bool* has_timeout, int* data, int data_size)
{
    uint8_t tmp = readb();
    int time = 0;
    *has_timeout = false;
    while(tmp != _PS2_DEVICE_ACK)
    {
        if(time > timeout)
        {
            *has_timeout = true;
            return;
        }
        time++;
        if(tmp == _PS2_DEVICE_RESEND)
        {
            for(int i = 0; i < data_size; i++)
            {
                writeb(_PS2_REGISTER_PORT, _PS2_SEND_SECOND);
                writeb(_PS2_DATA_PORT, data[i]);
                wait();
            }
        }
        tmp = readb();
    }
}

void ps2_init()
{
    buf1_len = 0;
    buf2_len = 0;
    inited = false;
    timeouted = false;
    printf("Initializing PS/2 controller...\n");
    // Disable devices so they don't send some random stuff 
    //  while initializing controller
    writeb(_PS2_REGISTER_PORT, _PS2_DISABLE_FIRST);
    writeb(_PS2_REGISTER_PORT, _PS2_DISABLE_SECOND);
    if(timeouted)
    {
        printf("Error!\nTimeout while waiting for input buffer to clear\n");
        return;
    }
    printf("Done!\n");
    // Flush output buffer in case there was a scancode
    flush();
    if(timeouted)
    {
        printf("Timeout while flushing the output buffer\n");
        printf("(Maybe buffer larger than %i)\n", timeout);
        return;
    }
    // Disable interrupts and translation
    writeb(_PS2_REGISTER_PORT, _PS2_READ_CONFIG_BYTE);
    config = readb();
    config &= ~(_PS2_CONFIG_FIRST_INT | _PS2_CONFIG_SECOND_INT 
                                                | _PS2_CONFIG_FIRST_TRANSLATE);
    
    writeb(_PS2_REGISTER_PORT, _PS2_WRITE_CONFIG_BYTE);
    writeb(_PS2_DATA_PORT, config);
    has_second = config & _PS2_CONFIG_SECOND_CLOCK;
    // should be set because we disabled, if it isn't, 
    // PS/2 controller definitely  doesn't support two channels.
    if(timeouted)
    {
        printf("Error!\nTimeout while setting configuration\n");
        return;
    }
    // Controller selftest
    writeb(_PS2_REGISTER_PORT, _PS2_TEST_CONTROLLER);
    uint8_t tmp = readb();
    if(timeouted)
    {
        printf("Error!\nTimeout while performing self-test\n");
        printf("Try increasing timeout in %s and recompiling\n", __FILE__);
        return;
    }
    if(tmp == _PS2_CONTROLLER_TEST_FAIL)
    {
        printf("Error!\nController self-test failed!\n");
        return;
    }
    else if(tmp != _PS2_CONTROLLER_TEST_PASS)
    {
        printf("Error!\nController replied with garbage: 0x%X\n", tmp);
        return;
    }
    
    // Check if config has been reset
    writeb(_PS2_REGISTER_PORT, _PS2_READ_CONFIG_BYTE);
    tmp = readb();
    if(tmp != config)
    {
        // Config has been reset
        // Do everything again
        writeb(_PS2_REGISTER_PORT, _PS2_DISABLE_FIRST);
        writeb(_PS2_REGISTER_PORT, _PS2_DISABLE_SECOND);
        // Flush again
        flush();
        writeb(_PS2_REGISTER_PORT, _PS2_WRITE_CONFIG_BYTE);
        writeb(_PS2_DATA_PORT, config);
        if(timeouted)
        {
            printf("Error!\nTimeout.\n");
            return;
        }
    }
    if(!has_second)
        printf("Controller has only one channel.\n");
    else
    {
        writeb(_PS2_REGISTER_PORT, _PS2_ENABLE_SECOND);
        writeb(_PS2_REGISTER_PORT, _PS2_READ_CONFIG_BYTE);
        config = readb();
        if(timeouted)
        {
            printf("Timeout while checking channel count\n");
            return;
        }
        // should not be set because we enabled, if it is, PS/2 controller
        // definitely doesn't support two channels.
        if(config & _PS2_CONFIG_SECOND_CLOCK)
        {
            has_second = false;
            printf("Controller has only one channel.\n");
        }
        else
        {
            has_second = true;
            printf("Controller has two channels.\n");
        }
    }
    uint8_t test;
    // now we know channel count for sure
    // test the actual ports
    writeb(_PS2_REGISTER_PORT, _PS2_TEST_FIRST);
    test = readb();
    if(timeouted)
    {
        printf("Timeout while testing first port\n");
        printf("Try increasing timeout in %s and recompiling\n", __FILE__);
        return;
    }
    if(test == _PS2_PORT_TEST_PASS)
    {
        first.works = true;
    }
    else
    {
        first.works = false;
        printf("First port is faulty: ");
        if(test == _PS2_PORT_TEST_CLOCK_LO)
            printf("Clock line stuck low\n");
        else if(test == _PS2_PORT_TEST_CLOCK_HI)
            printf("Clock line stuck high\n");
        else if(test == _PS2_PORT_TEST_DATA_LO)
            printf("Data line stuck low\n");
        else if(test == _PS2_PORT_TEST_DATA_HI)
            printf("Data line stuck high\n");
    }
    first.type = _PS2_TYPE_UNKNOWN;
    if(has_second)
    {
        writeb(_PS2_REGISTER_PORT, _PS2_TEST_SECOND);
        test = readb();
        if(timeouted)
        {
            printf("Timeout while testing second port\n");
            printf("Try increasing timeout in %s and recompiling\n", __FILE__);
            return;
        }
        if(test == _PS2_PORT_TEST_PASS)
        {
            second.works = true;
        }
        else
        {
            second.works = false;
            printf("Second port is faulty: ");
            if(test == _PS2_PORT_TEST_CLOCK_LO)
                printf("Clock line stuck low\n");
            else if(test == _PS2_PORT_TEST_CLOCK_HI)
                printf("Clock line stuck high\n");
            else if(test == _PS2_PORT_TEST_DATA_LO)
                printf("Data line stuck low\n");
            else if(test == _PS2_PORT_TEST_DATA_HI)
                printf("Data line stuck high\n");
        }
        second.type = _PS2_TYPE_UNKNOWN;
    }
    else
        second.works = false;
    
    if(first.works)
    {
        // now we want to identify what's on the ports
        // we start with the first one
        // enable it
        writeb(_PS2_REGISTER_PORT, _PS2_ENABLE_FIRST);
        // reset it, just in case BIOS
        writeb(_PS2_DATA_PORT, _PS2_DEVICE_RESET);
        wait();
        tmp = readb();
        wait();
        // ignore everything except pass, fail or timeout
        while(tmp != _PS2_DEVICE_TEST_PASS && tmp != _PS2_DEVICE_TEST_FAIL1 
            && tmp != _PS2_DEVICE_TEST_FAIL2 && !timeouted)
        {
            tmp = readb();
            wait();
        }
        
        if(timeouted)
        {
            printf("No device exists on first port.\n");
            writeb(_PS2_REGISTER_PORT, _PS2_DISABLE_FIRST);
            first.failed = true;
            timeouted = false;
        }
        else if(tmp == _PS2_DEVICE_TEST_FAIL1 || tmp == _PS2_DEVICE_TEST_FAIL2)
        {
            printf("Device on first port didn't pass self-check!\n");
            writeb(_PS2_REGISTER_PORT, _PS2_DISABLE_FIRST);
            first.failed = true;
        }
        else
        {
            first.failed = false;
            // now we disable the scanning for this boye in case it fucks up
            // with the other data
            writeb(_PS2_DATA_PORT, _PS2_DEVICE_DISABLE);
            wait_ack(true, _PS2_DEVICE_DISABLE);
            flush();
            if(timeouted)
            {
                timeouted = false;
                printf("Error!\nTimeout while waiting for ACK\n");
                writeb(_PS2_REGISTER_PORT, _PS2_DISABLE_FIRST);
            }
            else
            {
                writeb(_PS2_DATA_PORT, _PS2_DEVICE_IDENTIFY);
                wait_ack(true, _PS2_DEVICE_IDENTIFY);
                first.code.length = -1;
                while(!timeouted && first.code.length < 2)
                {
                    first.code.length++;
                    first.code.code[first.code.length] = readb();
                }
                timeouted = false;
                printf("Port 1 device: ");
                first.type = get_type(first);
                if(first.type == _PS2_TYPE_UNKNOWN)
                {
                    printf("First port device could not be identified,"
                                                        "disabling port.\n");
                    writeb(_PS2_REGISTER_PORT, _PS2_DISABLE_FIRST);
                }
            }
        }
    }
    
    if(second.works)
    {
        // now all the same for the second
        writeb(_PS2_REGISTER_PORT, _PS2_ENABLE_SECOND);
        writeb(_PS2_REGISTER_PORT, _PS2_SEND_SECOND);
        writeb(_PS2_DATA_PORT, _PS2_DEVICE_RESET);
        wait();
        tmp = readb();
        wait();
        // ignore everything except pass, fail or timeout
        while(tmp != _PS2_DEVICE_TEST_PASS && tmp != _PS2_DEVICE_TEST_FAIL1 &&
            tmp != _PS2_DEVICE_TEST_FAIL2 && !timeouted)
        {
            tmp = readb();
            wait();
        }
        
        if(timeouted)
        {
            printf("No device exists on second port.\n");
            writeb(_PS2_REGISTER_PORT, _PS2_DISABLE_SECOND);
            second.failed = true;
            timeouted = false;
        }
        else if(tmp == _PS2_DEVICE_TEST_FAIL1 || tmp == _PS2_DEVICE_TEST_FAIL2)
        {
            printf("Device on second port didn't pass self-check!\n");
            writeb(_PS2_REGISTER_PORT, _PS2_DISABLE_SECOND);
            second.failed = true;
        }
        else
        {
            second.failed = false;
            writeb(_PS2_REGISTER_PORT, _PS2_SEND_SECOND);
            writeb(_PS2_DATA_PORT, _PS2_DEVICE_DISABLE);
            wait_ack(false, _PS2_DEVICE_DISABLE);
            flush();
            if(timeouted)
            {
                printf("Error!\nTimeouted while waiting for ACK\n");
                writeb(_PS2_REGISTER_PORT, _PS2_DISABLE_SECOND);
                timeouted = false;
            }
            else
            {
                writeb(_PS2_REGISTER_PORT, _PS2_SEND_SECOND);
                writeb(_PS2_DATA_PORT, _PS2_DEVICE_IDENTIFY);
                wait_ack(false, _PS2_DEVICE_IDENTIFY);
                second.code.length = -1;
                while(!timeouted && second.code.length < 2)
                {
                    second.code.length++;
                    second.code.code[second.code.length] = readb();
                }
                timeouted = false;
                printf("Port 2 device: ");
                second.type = get_type(second);
                if(second.type == _PS2_TYPE_UNKNOWN)
                {
                    printf("Second port device could not be identified,"
                                                            " disabling.\n");
                    writeb(_PS2_REGISTER_PORT, _PS2_DISABLE_SECOND);
                }
            }
        }
    }
    
    if((first.failed || !first.works) && (second.failed || !second.works))
    {
        printf("No working devices detected. Initialization has not "
                                                                "succeeded\n");
        return;
    }
    
    if(first.works && !first.failed)
    {
        if(first.type == _PS2_TYPE_UNKNOWN_KB ||
            first.type == _PS2_TYPE_TRANSLATED_AT_KB || 
            first.type == _PS2_TYPE_TRANSLATED_MF2_KB || 
            first.type == _PS2_TYPE_MF2_KB)
        {
            irq_enable(1);
            printf("Initializing keyboard driver for first device.\n");
            config |= _PS2_CONFIG_FIRST_INT;
            writeb(_PS2_REGISTER_PORT, _PS2_WRITE_CONFIG_BYTE);
            writeb(_PS2_DATA_PORT, config);
            f.getscan = read_f;
            f.sendbyte = send_first;
            f.readbyte = read;
            f.wait = wait;
            f.wait_ack = wait_ack_f;
            f.translated = first.type == _PS2_TYPE_TRANSLATED_AT_KB ||
                                    first.type == _PS2_TYPE_TRANSLATED_MF2_KB;
            kb_add(&f);
        }
        else
        {
            printf("No suitable drivers for device on first port!\n");
            writeb(_PS2_REGISTER_PORT, _PS2_DISABLE_FIRST);
        }
    }
    
    if(second.works && !second.failed)
    {
        if(second.type == _PS2_TYPE_UNKNOWN_KB ||
            second.type == _PS2_TYPE_TRANSLATED_AT_KB || 
            second.type == _PS2_TYPE_TRANSLATED_MF2_KB || 
            second.type == _PS2_TYPE_MF2_KB)
        {
            irq_enable(12);
            printf("Initializing keyboard driver for second device.\n");
            config |= _PS2_CONFIG_SECOND_INT;
            writeb(_PS2_REGISTER_PORT, _PS2_WRITE_CONFIG_BYTE);
            writeb(_PS2_DATA_PORT, config);
            s.getscan = read_s;
            s.sendbyte = send_second;
            s.readbyte = read;
            s.wait = wait;
            s.wait_ack = wait_ack_s;
            s.translated = second.type == _PS2_TYPE_TRANSLATED_AT_KB ||
                                    second.type == _PS2_TYPE_TRANSLATED_MF2_KB;
            kb_add(&s);
        }
        else
        {
            printf("No suitable drivers for device on second port!\n");
            writeb(_PS2_REGISTER_PORT, _PS2_DISABLE_SECOND);
        }
    }
    
    printf("PS/2 initialized!\n\n");
    inited = true;
    kb_start();
}

bool ps2_status()
{
    return inited;
}

void ps2_first()
{
    uint8_t data = inb(_PS2_DATA_PORT);
    if(buf1_len >= 255)
        buf1_len = 254;
    memmove(buf1, buf1+1, buf1_len);
    buf1[0] = data;
    buf1_len++;
}

void ps2_second()
{
    uint8_t data = inb(_PS2_DATA_PORT);
    if(buf2_len >= 255)
        buf2_len = 254;
    memmove(buf2, buf2+1, buf2_len);
    buf2[0] = data;
    buf2_len++;
}
