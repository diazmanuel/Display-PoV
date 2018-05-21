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

#define AT(layer, x, y, k) \
    layer->data[(((y) % layer->h) * layer->w + ((x) % layer->w)) * 3 + (k)]

static bool color_is_black(const uint8_t *p)
{
    return p[0] == 0 && p[1] == 0 && p[2] == 0;
}

// XXX: empty layer shouldn't have to alloc memory.
layer_t *layer_create(int w, int h)
{
    layer_t *layer = calloc(1, sizeof(*layer));
    layer->w = w;
    layer->h = h;
    layer->data = calloc(1, w * h * 3);
    layer->visible = true;
    sprintf(layer->name, "Unnamed");
    layer->ref = calloc(1, sizeof(*layer->ref));
    *layer->ref = 1;
    return layer;
}

void layer_delete(layer_t* layer)
{
    if (!layer) return;
    (*layer->ref)--;
    if (*layer->ref == 0) {
        free(layer->data);
        free(layer->ref);
    }
    free(layer);
}

void layer_prepare_write(layer_t *layer)
{
    uint8_t *data;
    if (*layer->ref == 1) return;
    (*layer->ref)--;
    data = layer->data;
    layer->ref = calloc(1, sizeof(*layer->ref));
    *layer->ref = 1;
    layer->data = calloc(1, layer->w * layer->h * 3);
    memcpy(layer->data, data, layer->w * layer->h * 3);
}

layer_t *layer_copy(layer_t *other)
{
    layer_t *layer;
    layer = calloc(1, sizeof(*layer));
    *layer = *other;
    (*layer->ref)++;
    return layer;
}

void layer_clear(layer_t *layer)
{
    layer_prepare_write(layer);
    memset(layer->data, 0, 3 * layer->w * layer->h);
}

void layer_merge(layer_t *layer, const layer_t *other)
{
    int i;
    layer_prepare_write(layer);
    for (i = 0; i < layer->w * layer->h; i++) {
        if (color_is_black(&other->data[i * 3])) continue;
        memcpy(&layer->data[i * 3], &other->data[i * 3], 3);
    }
}

void layer_paint(layer_t *layer, int x, int y, uint8_t color[3],
                 float radius, float s, bool spherical)
{
    layer_prepare_write(layer);
    int i, j, k, xx, yy;
    float cx, cy, px, py, r, coef, bh, bw;

    if (spherical) {
        bw = radius * 2;
        bh = layer->h;
    } else {
        bw = bh = radius * 2;
    }

    // Point center in rect coordinates
    cx = x * cos((float)y / layer->h * M_PI * 2);
    cy = x * sin((float)y / layer->h * M_PI * 2);

    for (i = -bh; i < +bh; i++)
    for (j = -bw; j < +bw; j++) {
        xx = x + j;
        yy = y + i;
        if (xx < 0 || xx >= layer->w) continue;
        if (yy < 0 || yy >= layer->h) continue;
        if (!spherical) {
            r = sqrt(i * i + j * j);
        } else {
            px = cx - xx * cos((float)yy / layer->h * M_PI * 2);
            py = cy - xx * sin((float)yy / layer->h * M_PI * 2);
            r = sqrt(px * px + py * py);
        }
        if (s)
            coef = smoothstep(radius * (1 + s), radius * (1 - s), r);
        else
            coef = r <= radius ? 1 : 0;
        for (k = 0; k < 3; k++) {
            AT(layer, xx, yy, k) = mix(AT(layer, xx, yy, k), color[k], coef);
            // AT(layer, xx, yy, k) = AT(layer, xx, yy, k) / 64 * 85;
        }
    }
    // ordered_dithering(layer->data, layer->w, layer->h);
}

void layer_dither(layer_t *layer)
{
    layer_prepare_write(layer);
    effect_ordered_dithering(layer->data, layer->w, layer->h);
}

// Pretty stupid...
static const uint8_t CENTER_MAP[16][16] = {
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},   // 1
    {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0},   // 2
    {1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0},   // 3
    {1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0},   // 4
    {1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0},   // 5
    {1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0},   // 6
    {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0},   // 7
    {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0},   // 8
    {1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0},   // 9
    {1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0},   // 10
    {1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0},   // 11
    {1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0},   // 12
    {1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0},   // 13
    {1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0},   // 14
    {1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1},   // 15
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},   // 16
};

void layer_fix_center(layer_t *layer)
{
    const int s = 4;
    int i, j, k, d;
    layer_prepare_write(layer);
    for (j = 0; j < s; j++) {
        d = mix(0, 16, (float)j / s);
        for (i = 0; i < layer->h; i++) {
            if (CENTER_MAP[d][i % 16]) continue;
            for (k = 0; k < 3; k++) AT(layer, j, i, k) = 0;
        }
    }
}


/*
void layer_fix_center(layer_t *layer)
{
    int i, j, k, v;
    const int threshold = 48;
    layer_prepare_write(layer);
    for (i = 0; i < layer->h; i++)
    for (j = 0; j < layer->w; j++)
    for (k = 0; k < 3; k++) {
        v = AT(layer, j, i, k);
        v = clamp(v * j / (float)layer->w, min(threshold, v), 255);
        AT(layer, j, i, k) = v;
    }
}
*/

static void r2s(const layer_t *layer, float x, float y, float *r, float *a)
{
    *r = sqrt(x * x + y * y);
    *a = atan2(y, x) / (M_PI * 2) * layer->h;
    if (*a < 0) *a += layer->h;
}

static void s2r(const layer_t *layer, float r, float a, float *x, float *y)
{
    *x = r * cos(a / layer->h * M_PI * 2);
    *y = r * sin(a / layer->h * M_PI * 2);
}

void layer_paint_line(layer_t *layer, int x1, int y1, int x2, int y2,
                      uint8_t color[3], float radius, float smoothness,
                      bool spherical)
{
    int i, nb;
    float k, r1x, r1y, r2x, r2y, x, y;
    nb = 128; // Could compute this.
    if (!spherical) {
        for (i = 0; i < nb; i++) {
            k = (float)i / nb;
            layer_paint(layer, mix(x1, x2, k), mix(y1, y2, k), color,
                        radius, smoothness, false);
        }
    } else {
        s2r(layer, x1, y1, &r1x, &r1y);
        s2r(layer, x2, y2, &r2x, &r2y);
        for (i = 0; i < nb; i++) {
            k = (float)i / nb;
            r2s(layer, mix(r1x, r2x, k), mix(r1y, r2y, k), &x, &y);
            layer_paint(layer, x, y, color,
                        radius, smoothness, false);
        }
    }

}

void layer_paint_text(layer_t *layer, const char *text,
                      const uint8_t color[3])
{
    uint8_t *img;
    int w, h, i, j, k, ofs;
    layer_prepare_write(layer);
    img = font_print(text, &w, &h);
    ofs = layer->h - layer->h / 4 - w / 2;
    for (i = 0; i < h; i++)
    for (j = 0; j < w; j++)
    for (k = 0; k < 3; k++) {
        AT(layer, layer->w - i - 1, j + ofs, k) = img[i * w + j] ? color[k] : 0;
    }
    free(img);
}

void layer_paint_emoji(layer_t *layer, emoji_t *emoji)
{
    uint8_t *img;
    const float scale = 0.74;
    img = emoji_get_img(emoji);
    layer_set_rdata(layer, img, emoji->w, emoji->h, 4, scale);
    free(img);
}

void layer_set_data(layer_t *layer, uint8_t *data, int w, int h, int bpp,
                    int x, int y)
{
    int i, j;
    layer_prepare_write(layer);
    for (i = 0; i < layer->h; i++)
    for (j = 0; j < layer->w; j++) {
        memcpy(&layer->data[(i * layer->w + j) * 3],
               &data[((y + i) * w + x + j) * bpp],
               3);
    }
}

void interpolate2d(const uint8_t *data, int w, int h, int bpp,
                   float x, float y, uint8_t col[4])
{
    float   x1, x2, y1, y2;
    int     i, cx1, cx2, cy1, cy2;
    uint8_t q11[4], q12[4], q21[4], q22[4];

    x1 = floor(x);
    y1 = floor(y);
    x2 = x1 + 1;
    y2 = y1 + 1;
    cx1 = clamp((int)x1, 0, w - 1);
    cx2 = clamp((int)x2, 0, w - 1);
    cy1 = clamp((int)y1, 0, h - 1);
    cy2 = clamp((int)y2, 0, h - 1);
    memcpy(q11, &data[(cy1 * w + cx1) * bpp], bpp);
    memcpy(q12, &data[(cy2 * w + cx1) * bpp], bpp);
    memcpy(q21, &data[(cy1 * w + cx2) * bpp], bpp);
    memcpy(q22, &data[(cy2 * w + cx2) * bpp], bpp);
    for (i = 0; i < bpp; i++) {
        col[i] = (q11[i] * (x2 - x) * (y2 - y) +
                  q21[i] * (x - x1) * (y2 - y) +
                  q12[i] * (x2 - x) * (y - y1) +
                  q22[i] * (x - x1) * (y - y1)) / ((x2 - x1) * (y2 - y1));
    }
}

void layer_set_rdata(layer_t *layer, uint8_t *data, int w, int h, int bpp,
                     float scale)
{
    int i, j, k;
    float a, r, x, y, rtot;
    uint8_t col[4];
    layer_prepare_write(layer);
    rtot = min(w, h) / 2.0;

    for (i = 0; i < layer->h; i++)
    for (j = 0; j < layer->w; j++) {
        a = (float)i / layer->h * M_PI * 2;
        r = (float)j / layer->w;
        x = w / 2 + rtot * r * cos(a) / scale;
        y = h / 2 + rtot * r * sin(a) / scale;
        memset(col, 0, 4);
        col[3] = 255;
        if (x >= 0 && x < w && y >= 0 && y < h)
            interpolate2d(data, w, h, bpp, x, y, col);
        if (col[3] == 0) memset(col, 0, 4);
        for (k = 0; k < 3; k++)
            AT(layer, j, i, k) = col[k] / 64 * 85;
    }
}

void layer_set_data_from_layer(layer_t *layer, const layer_t *other)
{
    if (layer->data == other->data) return;
    (*layer->ref)--;
    if (*layer->ref == 0) {
        free(layer->data);
        free(layer->ref);
    }
    layer->data = other->data;
    layer->ref = other->ref;
    (*layer->ref)++;
}

void layer_get_color_at(const layer_t *layer, int x, int y, uint8_t out[3])
{
    int k;
    for (k = 0; k < 3; k++)
        out[k] = AT(layer, x, y, k);
}

void layer_translate(layer_t *layer, int x, int y)
{
    int i, j, k, w, h;
    layer_t *tmp;

    h = layer->h;
    w = layer->w;
    tmp = layer_copy(layer);
    layer_prepare_write(layer);

    for (i = 0; i < h; i++)
    for (j = 0; j < w; j++)
    for (k = 0; k < 3; k++) {
        AT(layer, j, i, k) = AT(tmp, (j + x + w) % w, (i + y + h) % h, k);
    }
    layer_delete(tmp);
}
