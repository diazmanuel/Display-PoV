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

/*
 * The files are saved as a big png image with all the layer data put
 * next to each other.  The first one contains binary data.
 * +------------+-----------+-----------+- - -
 * | Header     | data 1    | data 2    |
 * |            |           |           |
 * |            |           |           |
 * |            |           |           |
 * |            |           |           |
 * |            |           |           |
 * |            |           |           |
 * |            |           |           |
 * |            |           |           |
 * +------------+-----------+-----------+- - -
 *
 * Header format:
 *
 * y
 * --
 * 17:          S P I N     ; Magic string.
 * 18:          v w h n     ; version, width, height, nb layers.
 * 19+i*2+0:    name        ; layer name.
 * 19+i*2+1:    i           ; layer data index.
 */

// Image format version.
#define VERSION 1

static void fix_colors(uint8_t *data, int w, int h, int bpp, int hw)
{
    int i, j, k;
    for (i = 0;  i < h; i++)
    for (j = hw; j < w; j++)
    for (k = 0; k < 3; k++) {
        data[(i * w + j) * bpp + k] = data[(i * w + j) * bpp + k] / 64 * 85;
    }
}

static void data_put_bin(uint8_t *data, int data_w, int data_h,
                         int x, int y, int size, const char *msg)
{
    int i;
    for (i = 0; i < size; i++) {
        data[(y * data_w + x + i) * 3 + 0] = msg[i];
        data[(y * data_w + x + i) * 3 + 1] = 0;
        data[(y * data_w + x + i) * 3 + 2] = 0;
    }
}

static int data_get_bin(uint8_t *data, int data_w, int data_h,
                        int x, int y, int size, char *msg)
{
    int i;
    for (i = 0; i < size; i++) {
        msg[i] = data[(y * data_w + x + i) * 3 + 0];
    }
    return 0;
}

static void data_put_int(uint8_t *data, int data_w, int data_h,
                           int x, int y, int nb, ...)
{
    int i, v;
    uint8_t v8[2];
    va_list args;
    va_start(args, nb);
    for (i = 0; i < nb; i++) {
        v = va_arg(args, int);
        assert(v >= 0 && v < 65536); // For the moment.
        v8[0] = (v >> 0) & 0xff;
        v8[1] = (v >> 8) & 0xff;
        data[(y * data_w + x + i) * 3 + 0] = v8[0];
        data[(y * data_w + x + i) * 3 + 1] = v8[1];
    }
    va_end(args);
}

static int data_get_int(uint8_t *data, int data_w, int data_h,
                        int x, int y, int nb, ...)
{
    // XXX: check image size correct.
    int i, *v;
    uint8_t v8[2];
    va_list args;
    va_start(args, nb);
    for (i = 0; i < nb; i++) {
        v = va_arg(args, int*);
        v8[0] = data[(y * data_w + x + i) * 3 + 0];
        v8[1] = data[(y * data_w + x + i) * 3 + 1];
        *v = v8[0] + (v8[1] << 8);
    }
    va_end(args);
    return 0;
}

image_t * file_load(const char *path)
{
    int i, data_w, data_h, data_bpp = 0, version, w, h, hw, n;
    bool has_header;
    uint8_t *data = NULL;
    char magic[4];
    image_t *img = NULL;
    layer_t *layer;

    data = img_read(path, &data_w, &data_h, &data_bpp);
    if (!data) {
        LOG_E("Cannot open image %s", path);
        goto error;
    }
    if (data_w < 32 || data_h < 128 || data_bpp < 3) {
        LOG_E("Image at %s doesn't have correct size", path);
        goto error;
    }

    // Read the header.
    data_get_bin(data, data_w, data_h, 4, 17, 4, magic);
    has_header = (strncmp(magic, "SPINO", 4) == 0);
    if (has_header) {
        if (data_get_int(data, data_w, data_h, 4, 18, 4,
                         &version, &w, &h, &n))
            goto error;
        LOG_D("image v: %d w:%d h:%d n:%d", version, w, h, n);
        if (version != 1 || data_w % w || data_h != h || n == 0) {
            LOG_E("Corrupt image");
            goto error;
        }
        hw = w;
    } else {  // No header, assume 32x128 image, no layers name.
        LOG_E("Image has no header, assumes 32x128");
        h = data_h;
        w = 32;
        hw = 0;
        n = data_w / w;
    }
    LOG_D("w:%d h:%d n:%d bpp:%d", w, h, n, data_bpp);

    fix_colors(data, data_w, data_h, data_bpp, hw);
    img = image_create(w, h);
    asprintf(&img->path, "%s", path);

    for (i = 0; i < n; i++) {
        layer = image_add_layer(img);
        if (has_header) {
            data_get_bin(data, data_w, data_h, 4, 19 + i * 2 + 0,
                         ARRAY_SIZE(layer->name), layer->name);
            data_get_int(data, data_w, data_h, 4, 19 + i * 2 + 1,
                         1, &layer->data_i);
        } else {
            layer->data_i = i;
        }
        layer_set_data(layer, data, data_w, data_h, data_bpp,
                       hw + layer->data_i * w, 0);
    }
    image_compress(img);

error:
    free(data);
    return img;
}

static void data_blit(uint8_t *dst, int dstw, int dsth, int dstbpp,
                      const uint8_t *src, int srcw, int srch, int srcbpp,
                      int x, int y)
{
    int i, j, w, h;
    w = srcw;
    h = srch;
    assert(dstbpp == srcbpp);
    for (i = 0; i < h; i++)
    for (j = 0; j < w; j++) {
        memcpy(&dst[((i + y) * dstw + j + x) * dstbpp],
               &src[(i * srcw + j) * srcbpp], srcbpp);
    }
}

void file_save(image_t *img, const char *path)
{
    int i, data_w, data_h, w, h, nb_layers, nb_layer_data = 0;
    uint8_t *data;
    uint8_t *layer_datas[256] = {}; // All the unique layer data pointers.
    layer_t *layer;
    w = img->w;
    h = img->h;

    image_compress(img);
    // Count the number of layer data.
    nb_layers = 0;
    DL_FOREACH(img->layers, layer) {
        nb_layers++;
        for (i = 0; i < ARRAY_SIZE(layer_datas); i++) {
            if (!layer_datas[i]) {
                layer_datas[i] = layer->data;
                nb_layer_data++;
            }
            if (layer->data == layer_datas[i]) {
                layer->data_i = i;
                break;
            }
        }
    }

    data_w = w + nb_layer_data * w;
    data_h = h;
    data = calloc(1, data_w * data_h * 3);
    data_put_bin(data, data_w, data_h, 4, 17, 4, "SPIN");
    data_put_int(data, data_w, data_h, 4, 18, 4,
                 VERSION, w, h, nb_layers);
    i = 0;
    DL_FOREACH(img->layers, layer) {
        data_put_bin(data, data_w, data_h, 4, 19 + i * 2,
                     ARRAY_SIZE(layer->name), layer->name);
        data_put_int(data, data_w, data_h, 4, 19 + i * 2 + 1, 1,
                     layer->data_i);
        i++;
    }

    for (i = 0; i < nb_layer_data; i++) {
        data_blit(data, data_w, data_h, 3,
                  layer_datas[i], w, h, 3,
                  w + i * w, 0);
    }
    img_write(data, data_w, data_h, 3, path);
    free(data);
    if (path != img->path) {
        free(img->path);
        asprintf(&img->path, "%s", path);
    }
}

void file_import(image_t *img, const char *path)
{
    int data_w, data_h, data_bpp = 0;
    uint8_t *data;
    layer_t *layer;
    data = img_read(path, &data_w, &data_h, &data_bpp);
    if (!data) {
        LOG_E("Cannot read image %s", path);
        goto error;
    }
    layer = image_add_layer(img);
    layer_set_rdata(layer, data, data_w, data_h, data_bpp, 1.0);
error:
    free(data);
}
