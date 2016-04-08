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


#include <checker.h>

bool fatal;

bool is_eof(char c)
{
    return (c == '\0');
}

bool is_eof_stream(str_stream *stream)
{
    return is_eof(stream->str[stream->pos]);
}

bool is_eoln(char c)
{
    return (c == LF) || (c == CR);
}

bool is_eoln_stream(str_stream *stream)
{
    return is_eoln(stream->str[stream->pos]);
}

bool is_blank(char c)
{
    return (c == LF) || (c == CR) || (c == SPACE) || (c == TAB);
}

bool is_blank_stream(str_stream *stream)
{
    return is_blank(stream->str[stream->pos]);
}

void str_stream_error(str_stream *stream, int err)
{
    stream->last_err = err;
    if (fatal || stream->fatal)
    {
        fprintf(stderr, "%s\n", ERR_NAMES[err]);
        if (stream->fatal)
        {
            fprintf(stderr, "%s\n", "Invalid test");
            exit(CR_IT);
        }
        else
        {
            fprintf(stderr, "%s\n", "Presentation error");
            exit(CR_PE);
        }
    }
}

str_stream new_str_stream(char *str)
{
    str_stream res;
    res.str = str;
    res.pos = 0;
    res.last_err = 0;
    res.fatal = false;
    return res;
}

str_stream new_str_stream_file(char *filename)
{
    FILE *f = fopen(filename, "rb");
    if (!f)
    {
        fprintf(stderr, "%s\n", "Failed to create str_stream from file: couldn't open file");
        exit(CR_IE);
    }

    fseek(f, 0, SEEK_END);
    size_t f_length = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *str = malloc(f_length);
    if (!str)
    {
        fprintf(stderr, "%s\n", "Failed to create str_stream from file: couldn't allocate memory");
        exit(CR_IE);
    }

    fread(str, 1, f_length, f);
    fclose(f);

    return new_str_stream(str);
}

void seek_visible(str_stream *stream)
{
    for (;is_blank_stream(stream); ++stream->pos);
}

void seek_eoln(str_stream *stream)
{
    for (;is_blank_stream(stream) && !is_eoln_stream(stream); ++stream->pos);
}

void seek_newline(str_stream *stream)
{
    for (;is_eoln_stream(stream); ++stream->pos);
}

char *read_string(str_stream *stream)
{
    seek_visible(stream);
    if (!stream->str[stream->pos])
    {
        str_stream_error(stream, ERR_UNEXPECTED_EOF);
        return NULL;
    }
    char *begin = stream->str + stream->pos;
    size_t len = 0;
    for (;!is_blank(stream->str[stream->pos]); ++stream->pos) ++len;
    char *str = malloc(len + 1);
    if (!str)
    {
        fprintf(stderr, "%s\n", "Failed to read from str_stream: couldn't allocate memory");
        exit(CR_IE);
    }
    memcpy(str, begin, len);
    str[len] = (char)0;
    return str;
}
char *read_big_integer(str_stream *stream)
{
    char *str_int = read_string(stream);
    bool valid = true;
    for (char *ptr = str_int; *ptr; ++ptr)
    {
        if (((*ptr < '0') || (*ptr > '9')) && (*ptr != '-') && (*ptr != '+'))
        {
            valid = false;
            break;
        }
    }
    if (!valid)
    {
        str_stream_error(stream, ERR_INVALID_FORMAT);
        free(str_int);
        return NULL;
    }

    return str_int;
}

long long read_long_long(str_stream *stream)
{
    char *str_int = read_big_integer(stream);
    if (!str_int)
    {
        return 0;
    }
    long long res;
    if (!sscanf(str_int, "%lld", &res))
    {
        str_stream_error(stream, ERR_INVALID_FORMAT);
        free(str_int);
        return 0;
    }
    free(str_int);
    return res;
}

unsigned long long read_unsigned_long_long(str_stream *stream)
{
    char *str_int = read_big_integer(stream);
    if (!str_int)
    {
        return 0;
    }
    long long res;
    if (!sscanf(str_int, "%llu", &res))
    {
        str_stream_error(stream, ERR_INVALID_FORMAT);
        free(str_int);
        return 0;
    }
    free(str_int);
    return res;
}

long long read_integer_in_bounds(str_stream *stream, long long min_bound, long long max_bound)
{
    long long integer = read_long_long(stream);
    if (stream->last_err)
    {
        return 0;
    }
    if ((integer >= min_bound) && (integer <= max_bound))
    {
        return integer;
    }
    else
    {
        str_stream_error(stream, ERR_OUT_OF_BOUNDS);
        return 0;
    }
}

unsigned long long read_unsigned_integer_in_bounds(str_stream *stream, unsigned long long min_bound, unsigned long long max_bound)
{
    unsigned long long integer = read_unsigned_long_long(stream);
    if (stream->last_err)
    {
        return 0;
    }

    if ((integer >= min_bound) && (integer <= max_bound))
    {
        return integer;
    }
    else
    {
        str_stream_error(stream, ERR_OUT_OF_BOUNDS);
        return 0;
    }
}

long read_long(str_stream *stream)
{
    return (long)read_integer_in_bounds(stream, LONG_MIN, LONG_MAX);
}

unsigned long read_unsigned_long(str_stream *stream)
{
    return (unsigned long)read_unsigned_integer_in_bounds(stream, 0, ULONG_MAX);
}

int read_int(str_stream *stream)
{
    return (int)read_integer_in_bounds(stream, INT_MIN, INT_MAX);
}

unsigned int read_unsigned_int(str_stream *stream)
{
    return (unsigned int)read_unsigned_integer_in_bounds(stream, 0, UINT_MAX);
}

long double read_long_double(str_stream *stream)
{
    char *str_float = read_string(stream);
    if (!str_float)
    {
        return 0;
    }
    long double res;
    if (!sscanf(str_float, "%Lf", &res))
    {
        str_stream_error(stream, ERR_INVALID_FORMAT);
        free(str_float);
        return 0;
    }
    free(str_float);
    return res;
}

double read_double(str_stream *stream)
{
    return (double)read_long_double(stream);
}

float read_float(str_stream *stream)
{
    return (float)read_long_double(stream);
}

char read_character(str_stream *stream)
{
    for (; is_eoln(stream->str[stream->pos]); ++stream->pos);
    if (is_eof_stream(stream))
    {
        str_stream_error(stream, ERR_UNEXPECTED_EOF);
        return '\0';
    }
    return stream->str[stream->pos++];
}

void expect_eof(str_stream *stream)
{
    seek_visible(stream);
    if (!is_eof_stream(stream))
    {
        str_stream_error(stream, ERR_EOF_EXPECTED);
    }
}

void expect_eoln(str_stream *stream)
{
    seek_eoln(stream);
    if (!is_eoln_stream(stream))
    {
        str_stream_error(stream, ERR_EOLN_EXPECTED);
    }
    seek_newline(stream);
}

void checker_init_impl(int argc, char **argv, char *description)
{
    if (argc > 1)
    {
        if (!strcmp(argv[1], "--show-description"))
        {
            printf("%s\n", description);
            exit(0);
        }
    }
    if (argc <= 2)
    {
        fprintf(stderr, "%s\n", "At least two arguments required to run this checker");
        fprintf(stderr, "%s\n", "Use --show-description to get checker description");
        exit(CR_IE);
    }

    char *file_input_name = argv[1];
    char *file_output_name = argv[2];
    char *file_pattern_name = (argc > 3)?argv[3]:NULL;

    f_input = new_str_stream_file(file_input_name);
    f_input.fatal = true;

    f_output = new_str_stream_file(file_output_name);

    if (file_pattern_name)
    {
        f_pattern = new_str_stream_file(file_pattern_name);
    }
    else
    {
        f_pattern.str = malloc(1);
        f_pattern.str[0] = '\0';
        f_pattern.pos = 0;
        f_pattern.last_err = 0;
    }
    f_pattern.fatal = true; 

    fatal = true; 
}
