/* MIT License
 *
 * Copyright (c) 2017 spino.tech Guillaume Chereau & Raphael Seghier
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include "spino.h"

// Png encoded font data (64x64).
const uint8_t DATA_PNG[323] __attribute__((aligned(4))) =
{
    137,80,78,71,13,10,26,10,0,0,0,13,73,72,68,82,0,0,0,64,0,0,0,64,1,3,0,
    0,0,144,167,227,157,0,0,0,6,80,76,84,69,0,0,0,255,255,255,165,217,159,
    221,0,0,0,248,73,68,65,84,40,207,99,72,151,61,110,121,233,185,23,3,195,
    148,133,26,78,66,78,65,12,12,223,21,245,138,238,57,37,65,68,130,64,34,
    211,101,143,59,94,122,227,125,128,1,14,62,247,60,182,120,191,84,196,129,
    97,85,208,148,165,76,170,96,198,147,157,143,32,140,214,37,74,42,75,29,
    24,186,124,154,151,30,146,112,106,64,232,234,210,135,50,130,20,161,12,
    69,55,40,35,160,3,202,232,232,103,64,3,137,18,135,42,231,37,3,25,155,85,
    212,156,152,166,0,205,187,200,225,85,220,146,12,100,76,20,208,231,242,
    152,0,100,36,239,57,84,216,145,140,170,79,80,128,217,176,135,129,65,129,
    65,137,99,145,32,11,80,192,198,137,195,77,18,196,96,104,2,137,0,37,24,
    25,65,106,28,144,52,9,40,9,8,104,62,115,96,80,224,112,18,60,81,155,153,
    192,192,192,210,36,192,162,153,84,0,20,113,16,60,84,203,144,192,224,62,
    65,73,128,81,243,17,146,174,69,10,80,198,82,24,131,9,38,197,10,99,48,53,
    48,12,71,0,0,226,245,68,181,234,209,248,205,0,0,0,0,73,69,78,68,174,66,
    96,130,
};

typedef struct {
    int x;
    int y;
    int width;
} char_t;

static const char_t CHARS[256] = {
    ['a'] = {0, 0, 4},
    ['b'] = {5, 0, 4},
    ['c'] = {10, 0, 4},
    ['d'] = {15, 0, 4},
    ['e'] = {20, 0, 4},
    ['f'] = {25, 0, 4},
    ['g'] = {30, 0, 4},
    ['h'] = {35, 0, 4},
    ['i'] = {40, 0, 3},
    ['j'] = {44, 0, 4},
    ['k'] = {49, 0, 4},
    ['l'] = {54, 0, 4},
    ['m'] = {0, 6, 5},
    ['n'] = {6, 6, 4},
    ['o'] = {11, 6, 4},
    ['p'] = {16, 6, 4},
    ['q'] = {21, 6, 4},
    ['r'] = {26, 6, 4},
    ['s'] = {31, 6, 4},
    ['t'] = {36, 6, 5},
    ['u'] = {42, 6, 4},
    ['v'] = {47, 6, 5},
    ['w'] = {53, 6, 5},
    ['x'] = {0, 12, 5},
    ['y'] = {6, 12, 5},
    ['z'] = {12, 12, 4},
    [' '] = {17, 12, 2},
    ['0'] = {0, 18, 4},
    ['1'] = {5, 18, 4},
    ['2'] = {10, 18, 4},
    ['3'] = {15, 18, 4},
    ['4'] = {20, 18, 4},
    ['5'] = {25, 18, 4},
    ['6'] = {30, 18, 4},
    ['7'] = {35, 18, 4},
    ['8'] = {40, 18, 4},
    ['9'] = {45, 18, 4},
    [':'] = {0, 24, 4},
    ['.'] = {5, 24, 4},
    ['-'] = {10, 24, 4},
    ['/'] = {15, 24, 5},
    ['('] = {21, 24, 4},
    [')'] = {26, 24, 4},
    ['*'] = {31, 24, 4},
    ['{'] = {36, 24, 4},
    ['}'] = {41, 24, 4},
    ['['] = {46, 24, 4},
    [']'] = {51, 24, 4},
    [','] = {56, 24, 4},
    [';'] = {0, 30, 4},
    ['_'] = {5, 30, 4},
    ['>'] = {10, 30, 4},
    ['<'] = {15, 30, 4},
    ['!'] = {20, 30, 4},
    ['|'] = {25, 30, 4},
    ['='] = {30, 30, 4},
    ['\\'] = {35, 30, 5},
    ['#'] = {41, 30, 5},
    ['@'] = {47, 30, 4},
    ['?'] = {52, 30, 4},
    [0xaf] = {57, 30, 4},
    ['"'] = {0, 36, 3},
    ['\''] = {10, 36, 2},
    ['&'] = {5, 36, 4},
};

static uint8_t *g_data = NULL;

void font_init(void)
{
    assert(!g_data);
    int w, h, bpp = 0;
    g_data = img_read_from_mem((const char*)DATA_PNG, ARRAY_SIZE(DATA_PNG),
                      &w, &h, &bpp);
    assert(w == 64 && h == 64 && bpp == 3);
}

static int to_lower(int c)
{
    if (c >= 'A' && c <= 'Z') {
        c = c - 'A' + 'a';
    }
    return c;
}

static void get_text_size(const char *msg, int *w, int *h)
{
    int c;
    int x = 0, y = 0;

    *w = *h = 0;
    while ((c = *msg++)) {
        c = to_lower(c);
        if (c >= ARRAY_SIZE(CHARS) || CHARS[c].width == 0) continue;
        x += CHARS[c].width + 1;
        *w = max(*w, x);
        *h = max(*h, y + 5);
    }
    if (*w > 0) *w -= 1;
}

uint8_t *font_print(const char *msg, int *w, int *h)
{
    int c, x = 0, y = 0, i, j;
    const char_t *ch;
    uint8_t *ret;
    get_text_size(msg, w, h);
    ret = calloc(1, (*w) * (*h));

    while ((c = *msg++)) {
        c = to_lower(c);
        if (c >= ARRAY_SIZE(CHARS) || CHARS[c].width == 0) continue;
        ch = &CHARS[c];
        for (i = 0; i < 5; i++)
        for (j = 0; j < ch->width; j++) {
            ret[(i + y) * (*w) + j + x] =
                g_data[((ch->y + i) * 64 + ch->x + j) * 3];
        }
        x += ch->width + 1;
    }
    return ret;
}
