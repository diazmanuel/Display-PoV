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
#include "emoji_data.inl"

const char** emoji_get_categories(int *nb) {
    if (nb) *nb = ARRAY_SIZE(CATEGORIES) - 1;
    return CATEGORIES;
}

void emoji_iter(const char *category, int (*f)(emoji_t *emoji, int i))
{
    int i, r, count = 0;
    emoji_t *emoji;
    for (i = 0; i < ARRAY_SIZE(EMOJIES); i++) {
        emoji = &EMOJIES[i];
        if (strcmp(emoji->category, category) != 0) continue;
        r = f(emoji, count++);
        if (r) return;
    }
}

uint8_t *emoji_get_img(emoji_t *emoji)
{
    spinoedit_t *se = spinoedit();
    uint8_t *buf, *img;
    int w, h, bpp = 0;
    buf = malloc(emoji->data_size + 4);
    decode_85((char*)buf, emoji->data, strlen(emoji->data));
    assert(strncmp((const char*)buf + 1, "PNG", 3) == 0);
    img = img_read_from_mem((const char*)buf,
                            emoji->data_size, &w, &h, &bpp);
    free(buf);

    effect_hsl(img, w, h, bpp,
               se->emoji_hsl[0], se->emoji_hsl[1], se->emoji_hsl[2]);
    return img;
}

int emoji_get_texture(emoji_t *emoji)
{
    uint8_t *buf, *img;
    int w, h, bpp = 0;
    if (!emoji->texture) {
        buf = malloc(emoji->data_size + 4);
        decode_85((char*)buf, emoji->data, strlen(emoji->data));
        assert(strncmp((const char*)buf + 1, "PNG", 3) == 0);
        img = img_read_from_mem((const char*)buf,
                                       emoji->data_size, &w, &h, &bpp);
        assert(w == emoji->w && h == emoji->h);
        free(buf);

        GL(glGenTextures(1, &emoji->texture));
        GL(glBindTexture(GL_TEXTURE_2D, emoji->texture));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, img);
        free(img);
    }
    return emoji->texture;
}
