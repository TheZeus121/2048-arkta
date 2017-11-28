#
# Makefile for 2048/Arkta kernel
#

################################################################################
# MIT License                                                                  #
#                                                                              #
# Copyright (c) 2017 Uko Kokņevičs (Uko Koknevics)                             #
#                                                                              #
# Permission is hereby granted, free of charge, to any person obtaining a copy #
# of this software and associated documentation files (the "Software"), to     #
# deal in the Software without restriction, including without limitation the   #
# rights to use, copy, modify, merge, publish, distribute, sublicense, and/or  #
# sell copies of the Software, and to permit persons to whom the Software is   #
# furnished to do so, subject to the following conditions:                     #
#                                                                              # 
# The above copyright notice and this permission notice shall be included in   #
# all copies or substantial portions of the Software.                          #
#                                                                              #
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR   #
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,     #
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE  #
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER       #
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING      #
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS #
# IN THE SOFTWARE.                                                             #
################################################################################

CC=i386-elf-gcc

SOURCES=src/boot.o src/main.o src/gdt.o src/lgdt.o src/idt.o src/lidt.o \
src/irq.o src/ps2.o src/keyboard.o src/ports.o src/string.o src/stdio.o \
src/vfprintf.o

CFLAGS=-nostdlib -fno-builtin -fno-stack-protector -std=gnu99 -ffreestanding \
-Wall -Wextra -I./include -O2 -D__kernel__
LDFLAGS=-Tlinker.ld
ASFLAGS=-felf32

all: $(SOURCES) link

clean:
	-rm src/*.o kernel

link:
	$(CC) $(LDFLAGS) $(CFLAGS) -o kernel $(SOURCES) -lgcc

.s.o:
	nasm $(ASFLAGS) $<


.PHONY: all clean link
