//
// _ports.h - communication with ports for kernel/libk
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

#ifndef __kernel__
#error This can be used only in kernel mode!
#else

#ifndef __PORTS_H
#define __PORTS_H

#include <stdint.h>

// 0x0020 - 0x003F -- PIC 1 (Programmable Interrupt Controller 8259)
#define _PIC_MASTER_COMMAND         0x0020
#define _PIC_READ_ISR               0x0B
#define _PIC_INIT                   0x11
#define _PIC_EOI                    0x20
#define _PIC_MASTER_DATA            0x0021
#define _PIC_8086_MODE              0x01

// 0x0040 - 0x005F -- PIT (Programmable Interrupt Timer 8253, 8254)
#define _PIT_CHANNEL0_DATA          0x0040
#define _PIT_CHANNEL1_DATA          0x0041
#define _PIT_CHANNEL2_DATA          0x0042
#define _PIT_COMMAND_REGISTER       0x0043

#define _PIT_SELECT_CHAN0           0
#define _PIT_SELECT_CHAN1           (1<<6)
#define _PIT_SELECT_CHAN2           (2<<6)
// 3<<6 is readback command only on 8254
// 0<<4 is latch count command, not going to use it
#define _PIT_ACCESS_LOW             (1<<4)
#define _PIT_ACCESS_HIGH            (2<<4)
#define _PIT_ACCESS_BOTH            (3<<4)
#define _PIT_INTERRUPT_ON_COUNT     0
#define _PIT_HW_RETRIG_ONESHOT      (1<<1)
#define _PIT_RATE_GENERATOR         (2<<1)
#define _PIT_SQUARE_WAVE_GENERATOR  (3<<1)
#define _PIT_SW_TRIGGERED_STROBE    (4<<1)
#define _PIT_HW_TRIGGERED_STROBE    (5<<1)
// 6 and 7 are the same as 2 and 3 respectively
// x<<1 is decimal or binary, not gonna touch decimal, binary is 0 so default

// 0x0060 - 0x006F -- Keyboard Controller 804x (8041, 8042)
#define _PS2_DATA_PORT              0x0060
#define _PS2_DEVICE_IDENTIFY        0xF2
#define _PS2_DEVICE_DISABLE         0xF5
#define _PS2_DEVICE_RESEND          0xFE
#define _PS2_DEVICE_RESET           0xFF
#define _PS2_DEVICE_ACK             0xFA
#define _PS2_DEVICE_TEST_PASS       0xAA
#define _PS2_DEVICE_TEST_FAIL1      0xFC
#define _PS2_DEVICE_TEST_FAIL2      0xFD

// I am going to pretty much ignore this one, but I have it here for turning 
// speaker on or off
#define _PS2_KB_CONTROLLER_PORT_B   0x0061
#define _PIT_CHAN2_GATE_ENABLE      (1<<0)
#define _PS2_SPEAKER_DATA_ENABLE    (1<<1)
#define _SPEAKER_ENABLE             3

#define _PS2_REGISTER_PORT          0x0064  /* Write - command, Read - status */
#define _PS2_STATUS_OUT_BUFFER      (1<<0)  /* If set, full */
#define _PS2_STATUS_IN_BUFFER       (1<<1)  /* If set, full */
#define _PS2_STATUS_SYSTEM_FLAG     (1<<2)  /* Should be set by hardware */
#define _PS2_STATUS_CMD_OR_DATA     (1<<3)  /* Data in buf is for: */
                                            // if 0 -- for PS/2 device
                                            // if 1 -- for PS/2 controller
#define _PS2_STATUS_TIMEOUT_ERR     (1<<6)  /* If set, error */
#define _PS2_STATUS_PARITY_ERR      (1<<7)  /* If set, error */
#define _PS2_READ_CONFIG_BYTE       0x20
#define _PS2_WRITE_CONFIG_BYTE      0x60
#define _PS2_CONFIG_FIRST_INT       (1<<0)  /* If set, enabled */
#define _PS2_CONFIG_SECOND_INT      (1<<1)  /* If set, enabled */
#define _PS2_CONFIG_SYSTEM_FLAG     (1<<2)  /* Should be set by hardware */
#define _PS2_CONFIG_FIRST_CLOCK     (1<<4)  /* If set, disabled */
#define _PS2_CONFIG_SECOND_CLOCK    (1<<5)  /* If set, disabled */
#define _PS2_CONFIG_FIRST_TRANSLATE (1<<6)  /* If set, enabled */
// There is no translation for second
#define _PS2_DISABLE_SECOND         0xA7
#define _PS2_ENABLE_SECOND          0xA8
#define _PS2_TEST_SECOND            0xA9
#define _PS2_PORT_TEST_PASS         0x00
#define _PS2_PORT_TEST_CLOCK_LO     0x01
#define _PS2_PORT_TEST_CLOCK_HI     0x02
#define _PS2_PORT_TEST_DATA_LO      0x03
#define _PS2_PORT_TEST_DATA_HI      0x04
#define _PS2_TEST_CONTROLLER        0xAA
#define _PS2_CONTROLLER_TEST_PASS   0x55
#define _PS2_CONTROLLER_TEST_FAIL   0xFC
#define _PS2_TEST_FIRST             0xAB
#define _PS2_DISABLE_FIRST          0xAD
#define _PS2_ENABLE_FIRST           0xAF
#define _PS2_SEND_SECOND            0xD4

// 0x00A0 - 0x00AF -- PIC 2 (Programmable Interrupt Controller 8259)
#define _PIC_SLAVE_COMMAND          0x00A0
#define _PIC_SLAVE_DATA             0x00A1

// 0x03D0 - 0x3DF -- CGA (Color Graphics Adapter)
#define _VGA_SELECT_REGISTER        0x03D4
#define _VGA_REGISTER_CURSORSTART   0x0A
#define _VGA_DATA_CURSOROFF         (1<<5)
#define _VGA_REGISTER_CURSOREND     0x0B
#define _VGA_REGISTER_CURSORLOCHIGH 0x0E
#define _VGA_REGISTER_CURSORLOCLOW  0x0F
#define _VGA_DATA_REGISTER          0x03D5

uint8_t     inb (uint16_t port);
uint16_t    inw (uint16_t port);
void        outb(uint16_t port, uint8_t value);

void        io_wait();

// __PORTS_H
#endif
// __kernel__
#endif
