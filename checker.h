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


#ifndef CHECKER_H
#define CHECKER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>

#ifndef max
    #define max(a, b) (((a) > (b))?(a):(b))
#endif

#ifndef min
    #define min(a, b) (((a) < (b))?(a):(b))
#endif

#define checker_main() main(int argc, char *argv[])

/* OK */
#define CR_OK 0

/* Wrong Answer */
#define CR_WA 1

/* Presentation Error */
#define CR_PE 2

/* Invalid Test */
#define CR_IT 3

/* Internal Error*/
#define CR_IE 127

/* Long long printf format */
#ifdef __MINGW32__
    #define LLD_FORMAT   "%I64d"
    #define LLU_FORMAT   "%I64u"
    #define LF_FORMAT    "%lf"
    typedef double LDOUBLE_CAST; 
#else
    #define LLD_FORMAT   "%lld"
    #define LLU_FORMAT   "%llu"
    #define LF_FORMAT    "%Lf"
    typedef long double LDOUBLE_CAST; 
#endif

#define ERR_UNEXPECTED_EOF 1
#define ERR_OUT_OF_BOUNDS 2
#define ERR_INVALID_FORMAT 3
#define ERR_EOF_EXPECTED 4
#define ERR_EOLN_EXPECTED 5

const char *ERR_NAMES[] = {
    "OK",
    "ERR_UNEXPECTED_EOF",
    "ERR_OUT_OF_BOUNDS",
    "ERR_INVALID_FORMAT",
    "ERR_EOF_EXPECTED",
    "ERR_EOLN_EXPECTED"
};

#define LF (char)10
#define CR (char)13
#define TAB (char)9
#define SPACE (char)' '

/* str_stream */

struct str_stream_t
{
    char *str;
    int pos;
    int last_err;
    bool fatal;
};
typedef struct str_stream_t str_stream;
str_stream new_str_stream(char *str);
str_stream new_str_stream_file(char *filename);

/* global str_stream objects */
str_stream f_input, f_output, f_pattern;
void checker_init_impl(int argc, char **argv, char *description);
#define checker_init(msg) checker_init_impl(argc, argv, msg)

/* Reading integers */
long long read_integer_in_bounds(str_stream *stream, long long min_bound, long long max_bound);

int read_int(str_stream *stream);
long read_long(str_stream *stream);
long long read_long_long(str_stream *stream);


unsigned long long read_unsigned_integer_in_bounds(str_stream *stream, unsigned long long min_bound, unsigned long long max_bound);

unsigned int read_unsigned_int(str_stream *stream);
unsigned long read_unsigned_long(str_stream *stream);
unsigned long long read_unsigned_long_long(str_stream *stream);

char *read_big_integer(str_stream *stream);

/* Reading characters */
char read_character(str_stream *stream);

/* Reading floating-point */
float read_float(str_stream *stream);
double read_double(str_stream *stream);
long double read_long_double(str_stream *stream);

/* Reading strings */
char *read_string(str_stream *stream);
char *read_line(str_stream *stream);

/* Utility */
bool is_eof(char c);
bool is_eof_stream(str_stream *stream);
bool is_eoln(char c);
bool is_eoln_stream(str_stream *stream);
bool is_blank(char c);
bool is_blank_stream(str_stream *stream);
void seek_visible(str_stream *stream);
void seek_eoln(str_stream *stream);
void str_stream_error(str_stream *stream, int err);
void expect_eof(str_stream *stream);
void expect_eoln(str_stream *stream);


/* Generators */
void generator_init_impl(int argc, char **argv, char *checker_string);
#define generator_init(checker_string) generator_init_impl(argc, argv, checker_string)

/* Randomness: xorshift* */

unsigned long long rnd_seed = 293432936541724496LL;

unsigned long long ask_rand();
unsigned long long ask_rand_in_bounds(unsigned long long lower, unsigned long long higher);
long long ask_weighted_rand_in_bounds(long long lower, long long higher, int weight);
double ask_rand_double();

void randomize(int x);

char* ask_rand_pattern(char *pattern);
/*
 * pattern examples:
 * [A-Z] - one any uppercase English letter
 * [A-Z]{5} - five any uppercase English letters
 * [A-Z]{1,100} - from 1 to 100 any uppercase English letters
 * [0-9a-f]{16} - random 256 bit hex number
 * (<pattern>){2} - repeat <pattern> 2 times
 * |a|b - empty string, 'a' or 'b'
 *
 * */

#endif // CHECKER_H
