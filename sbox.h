/*
 * Copyright (c) 2016 Tsarev Nikita
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#ifndef LIBEXEC_H
#define LIBEXEC_H

#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ptrace.h>
#include <sys/resource.h>
#include <asm-generic/resource.h>

#ifndef max
    #define max(a,b) ((a) > (b) ? (a) : (b))
#endif

#define MAX_BUF 1024

const int ER_OK = 0;
const int ER_TL = 1;
const int ER_RE = 2;
const int ER_ML = 4;
const int ER_IE = 8;
const int ER_SV = 16;

const int PF_ML = 0x4280;
const int PF_TL = 0x4282;
const int PF_SV = 0x4281;

const int EC_ML = 0x10000;
const int EC_TL = 0x40000;
const int EC_SV = 0x20000;

struct exec_result_t
{
    int flags;
    int exitcode;
    char *pstdout;
};

typedef struct exec_result_t exec_result;

void set_process_flag(int flag);
void set_memory_limit(int kb);
void set_cpu_limit(int ms);
void set_security(void);

exec_result run_limited_process(char *exe, char *input, int timeLimit, int memoryLimit);

#endif
