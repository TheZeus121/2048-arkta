//
// main.c - C-code entry
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

#include "gdt.h"
#include "idt.h"
#include "irq.h"
#include "keyboard.h"
#include "ps2.h"
#include "stdio.h"

static uint32_t next = 1;

uint32_t rand()
{
    next = next * 1103515245 + 12345;
    return (uint32_t)(next / 65536) % 32768;
}

void srand(uint32_t seed)
{
    next = seed;
}

void main()
{
    _text_init();
    printf("Welcome to 2048/Arkta! :D\n");
    printf("Loading, please wait...\n");
    gdt_init();
    idt_init();
    irq_init();
    ps2_init();
    if (!ps2_status())
    {
        printf("Failed to initialize PS/2 :(\n");
        printf("Will not start game because it requires input\n");
        for(;;);
    }
    printf("Generating a random number...\n");
    // check if we have hardware random available
    
    int available = 2;
    
    asm volatile (
        "movl $1, %%eax\n"
        "movl $0, %%ecx\n"
        "cpuid\n"
        "shr $30, %%ecx\n"
        "and $1, %%ecx\n"
        : "=c" (available)
        :
        : "eax"
    );
    
    if (available != 1)
    {
        printf("Hardware generated random numbers not available :(\n");
        srand(420);
        rand();
        rand();
    }
    else
    {
        printf("Hardware generated random number is available, generating\n");
        int seed = 420;
        asm volatile (
            "begin: "
            "rdrand %%eax\n"
            "jnc begin"
            : "=a" (seed)
        );
        srand(seed);
        rand();
        rand();
    }
    asm("sti");
    
    int data[4][4]; // we store the exponent not the product here
                    // so 1, 2, 3, 4 not 2, 4, 8, 16
    
    // initialize the data
    
    for (int x = 0; x < 4; x++)
        for (int y = 0; y < 4; y++)
            data[x][y] = 0;
    
    int a = rand() % 4;
    int b = rand() % 4;
    int c = rand() % 4;
    int d = rand() % 4;
    while (a == c && b == d)
    {
        c = rand() % 4;
        d = rand() % 4;
    }
    data[a][b] = 1;
    data[c][d] = 1;
    
    bool changed = true;
    bool lost = false;
    bool won = false;
    
    uint64_t score = 0;
    uint64_t highscore = 0;
    
    for (;;)
    {
        if (changed)
        {
            _text_drawfield(data, lost, won, score, highscore);
            changed = false;
        }
        
        bool spawn = false;
        
        kb_update();
        if (kb_ispressed(KEY_B))
        {
            _text_switchstyle();
            changed = true;
        }
        else if (kb_ispressed(KEY_R))
        {
            for (int x = 0; x < 4; x++)
                for (int y = 0; y < 4; y++)
                    data[x][y] = 0;
            
            int a = rand() % 4;
            int b = rand() % 4;
            int c = rand() % 4;
            int d = rand() % 4;
            while (a == c && b == d)
            {
                c = rand() % 4;
                d = rand() % 4;
            }
            data[a][b] = 1;
            data[c][d] = 1;
            lost = false;
            won = false;
            score = 0;
            changed = true;
        }
        else if (kb_ispressed(KEY_RIGHT))
        {
            for (int x = 0; x < 4; x++)
            {
                bool matched = false;
                for (int y = 2; y >= 0; y--)
                {
                    if (data[x][y] != 0)
                        for (int y2 = 3; y2 > y; y2--)
                        {
                            if (data[x][y2] == 0)
                            {
                                data[x][y2] = data[x][y];
                                data[x][y] = 0;
                                matched = false;
                                y2 = y;
                                spawn = true;
                            }
                            else if (!matched && data[x][y2] == data[x][y])
                            {
                                bool legit = true;
                                for (int i = y2 - 1; i > y; i--)
                                {
                                    if (data[x][i] != 0)
                                    {
                                        legit = false;
                                        i = y;
                                    }
                                }
                                if(legit)
                                {
                                    data[x][y2]++;
                                    score += 1 << data[x][y2];
                                    if (data[x][y2] == 11)
                                        won = true;
                                    data[x][y] = 0;
                                    matched = true;
                                    y2 = y;
                                    spawn = true;
                                }
                            }
                        }
                }
            }
        }
        else if (kb_ispressed(KEY_LEFT))
        {
            for (int x = 0; x < 4; x++)
            {
                bool matched = false;
                for (int y = 1; y < 4; y++)
                {
                    if (data[x][y] != 0)
                        for (int y2 = 0; y2 < y; y2++)
                        {
                            if (data[x][y2] == 0)
                            {
                                data[x][y2] = data[x][y];
                                data[x][y] = 0;
                                matched = false;
                                y2 = y;
                                spawn = true;
                            }
                            else if (!matched && data[x][y2] == data[x][y])
                            {
                                bool legit = true;
                                for (int i = y2 + 1; i < y; i++)
                                {
                                    if (data[x][i] != 0)
                                    {
                                        legit = false;
                                        i = y;
                                    }
                                }
                                if(legit)
                                {
                                    data[x][y2]++;
                                    score += 1 << data[x][y2];
                                    if (data[x][y2] == 11)
                                        won = true;
                                    data[x][y] = 0;
                                    matched = true;
                                    y2 = y;
                                    spawn = true;
                                }
                            }
                        }
                }
            }
        }
        else if (kb_ispressed(KEY_DOWN))
        {
            for (int y = 0; y < 4; y++)
            {
                bool matched = false;
                for (int x = 2; x >= 0; x--)
                {
                    if (data[x][y] != 0)
                        for (int x2 = 3; x2 > x; x2--)
                        {
                            if (data[x2][y] == 0)
                            {
                                data[x2][y] = data[x][y];
                                data[x][y] = 0;
                                matched = false;
                                x2 = x;
                                spawn = true;
                            }
                            else if (!matched && data[x2][y] == data[x][y])
                            {
                                bool legit = true;
                                for (int i = x2 - 1; i > x; i--)
                                {
                                    if (data[i][y] != 0)
                                    {
                                        legit = false;
                                        i = x;
                                    }
                                }
                                if(legit)
                                {
                                    data[x2][y]++;
                                    score += 1 << data[x2][y];
                                    if (data[x2][y] == 11)
                                        won = true;
                                    data[x][y] = 0;
                                    matched = true;
                                    x2 = x;
                                    spawn = true;
                                }
                            }
                        }
                }
            }
        }
        else if (kb_ispressed(KEY_UP))
        {
            for (int y = 0; y < 4; y++)
            {
                bool matched = false;
                for (int x = 1; x < 4; x++)
                {
                    if (data[x][y] != 0)
                        for (int x2 = 0; x2 < x; x2++)
                        {
                            if (data[x2][y] == 0)
                            {
                                data[x2][y] = data[x][y];
                                data[x][y] = 0;
                                matched = false;
                                x2 = x;
                                spawn = true;
                            }
                            else if (!matched && data[x2][y] == data[x][y])
                            {
                                bool legit = true;
                                for (int i = x2 + 1; i < x; i++)
                                {
                                    if (data[i][y] != 0)
                                    {
                                        legit = false;
                                        i = x;
                                    }
                                }
                                if(legit)
                                {
                                    data[x2][y]++;
                                    score += 1 << data[x2][y];
                                    if (data[x2][y] == 11)
                                        won = true;
                                    data[x][y] = 0;
                                    matched = true;
                                    x2 = x;
                                    spawn = true;
                                }
                            }
                        }
                }
            }
        }
        
        if(spawn)
        {
            int count = 0;
            int empties[16];
            for (int x = 0; x < 4; x++)
            {
                for (int y = 0; y < 4; y++)
                {
                    if (data[x][y] == 0)
                    {
                        empties[count] = x * 4 + y;
                        count++;
                    }
                }
            }
            
            if (count > 0)
            {
                int choice = empties[rand() % count];
                if (rand() % 10)
                    data[choice / 4][choice % 4] = 1;
                else
                    data[choice / 4][choice % 4] = 2;
            }
            
            changed = true;
        }
        
        bool not_lost = false;
        for (int x = 0; x < 3; x++)
        {
            for (int y = 0; y < 4; y++)
            {
                if (data[x][y] == data[x + 1][y] || data[x][y] == 0)
                {
                    not_lost = true;
                    y = 4;
                    x = 3;
                }
            }
        }
        if(!not_lost)
        {
            for (int x = 0; x < 4; x++)
            {
                for (int y = 0; y < 3; y++)
                {
                    if (data[x][y] == data[x][y + 1] || data[x][y] == 0)
                    {
                        not_lost = true;
                        y = 3;
                        x = 4;
                    }
                }
            }
        }
        lost = !not_lost;
        
        if(score > highscore)
        {
            highscore = score;
        }
    }
}
