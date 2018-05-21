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

static void hsl_to_rgb_f(const float hsl[3], float rgb[3])
{
    float r = 0, g = 0, b = 0, c, x, m;
    const float h = hsl[0] / 60, s = hsl[1], l = hsl[2];
    c = (1 - fabs(2 * l - 1)) * s;
    x = c * (1 - fabs(fmod(h, 2) - 1));
    if      (h < 1) {r = c; g = x; b = 0;}
    else if (h < 2) {r = x; g = c; b = 0;}
    else if (h < 3) {r = 0; g = c; b = x;}
    else if (h < 4) {r = 0; g = x; b = c;}
    else if (h < 5) {r = x; g = 0; b = c;}
    else if (h < 6) {r = c; g = 0; b = x;}
    m = l - 0.5 * c;
    rgb[0] = r + m;
    rgb[1] = g + m;
    rgb[2] = b + m;
}

static void rgb_to_hsl_f(const float rgb[3], float hsl[3])
{
    float h = 0, s, v, m, c, l;
    const float r = rgb[0], g = rgb[1], b = rgb[2];
    v = max(max(r, g), b);
    m = min(min(r, g), b);
    l = (v + m) / 2;
    c = v - m;
    if (c == 0) {
        h = s = 0;
    } else {
        if      (v == r) {h = (g - b) / c + (g < b ? 6 : 0);}
        else if (v == g) {h = (b - r) / c + 2;}
        else if (v == b) {h = (r - g) / c + 4;}
        h *= 60;
        s = (l > 0.5) ? c / (2 - v - m) : c / (v + m);
    }
    hsl[0] = h;
    hsl[1] = s;
    hsl[2] = l;
}

void rgb_to_hsl(const uint8_t rgb[3], uint8_t hsl[3])
{
    // XXX: use an optimized function that use int operations.
    float rgb_f[3] = {rgb[0] / 255.f, rgb[1] / 255.f, rgb[2] / 255.f};
    float hsl_f[3];
    rgb_to_hsl_f(rgb_f, hsl_f);
    hsl[0] = round(hsl_f[0] * 255 / 360);
    hsl[1] = round(hsl_f[1] * 255);
    hsl[2] = round(hsl_f[2] * 255);
}

void hsl_to_rgb(const uint8_t hsl[3], uint8_t rgb[3])
{
    // XXX: use an optimized function that use int operations.
    float hsl_f[3] = {hsl[0] / 255.f * 360, hsl[1] / 255.f, hsl[2] / 255.f};
    float rgb_f[3];
    hsl_to_rgb_f(hsl_f, rgb_f);
    rgb[0] = round(rgb_f[0] * 255);
    rgb[1] = round(rgb_f[1] * 255);
    rgb[2] = round(rgb_f[2] * 255);
}

static const uint8_t MAP[8][8] = {
    { 1, 49, 13, 61,  4, 52, 16, 64},
    {33, 17, 45, 29, 36, 20, 48, 32},
    { 9, 57,  5, 53, 12, 60,  8, 56},
    {41, 25, 37, 21, 44, 28, 40, 24},
    { 3, 51, 15, 63,  2, 50, 14, 62},
    {35, 19, 47, 31, 34, 18, 46, 30},
    {11, 59,  7, 55, 10, 58,  6, 54},
    {43, 27, 39, 23, 42, 26, 38, 22},
};

// In place ordered dithering.
void effect_ordered_dithering(uint8_t *img, int w, int h)
{
    int i, j, k, v;
    for (i = 0; i < h; i++)
    for (j = 0; j < w; j++)
    for (k = 0; k < 3; k++) {
        v = img[(i * w + j) * 3 + k];
        v = v + v * MAP[i % 8][j % 8] / 65;
        v = v / 64 * 85;
        img[(i * w + j) * 3 + k] = clamp(v, 0, 255);
    }
}

static void move(float *x, float v, float range)
{
    float dst = v >= 0 ? range : 0;
    v = fabs(v);
    *x = mix(*x, dst, v);
}

void effect_hsl(uint8_t *img, int w, int h, int bpp,
                float hue, float sat, float light)
{
    int i;
    float c[3];
    for (i = 0; i < w * h; i++) {
        c[0] = img[i * bpp + 0] / 255.f;
        c[1] = img[i * bpp + 1] / 255.f;
        c[2] = img[i * bpp + 2] / 255.f;
        rgb_to_hsl_f(c, c);
        c[0] = fmod(c[0] + hue, 360);
        move(&c[1], sat, 1);
        move(&c[2], light, 1);
        hsl_to_rgb_f(c, c);
        img[i * bpp + 0] = clamp(c[0] * 255, 0, 255);
        img[i * bpp + 1] = clamp(c[1] * 255, 0, 255);
        img[i * bpp + 2] = clamp(c[2] * 255, 0, 255);
    }
}
