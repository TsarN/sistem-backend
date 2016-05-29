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
#include <stdbool.h>

#ifndef __linux__
/* Cygwin */
#include <windows.h>
#include <Psapi.h>
#else
/* Linux */
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ptrace.h>
#include <sys/resource.h>
#include <asm-generic/resource.h>
#endif

#ifndef max
    #define max(a,b) ((a) > (b) ? (a) : (b))
#endif

#define MAX_BUF 1024

#define ER_OK 0
#define ER_TL 1
#define ER_RE 2
#define ER_ML 4
#define ER_IE 8
#define ER_SV 16

#ifdef __linux__

#define PF_ML 0x4280
#define PF_TL 0x4282
#define PF_SV 0x4281

#define EC_ML 0x10000
#define EC_TL 0x40000
#define EC_SV 0x20000

#endif

struct exec_result_t
{
    int flags;
    int exitcode;
    char *pstdout;
};

typedef struct exec_result_t exec_result;

exec_result run_limited_process(char *exe, char *input, int timeLimit, int memoryLimit, char **args);

#endif
