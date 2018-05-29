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

static spinoedit_t *g_spinoedit = NULL;

spinoedit_t *spinoedit(void)
{
    return g_spinoedit;
}

void spinoedit_init(spinoedit_t *spinoedit)
{
    spinoedit_t *se;
    storage_init();
    bluetooth_init();
    se = g_spinoedit = spinoedit;
    se->zoom = 6;
    se->zoom2 = 1;
    se->symmetry = 1;
    se->size = 1;
    se->color[0] = se->color[1] = se->color[2] = 255;
    se->emoji_hsl[1] = 1.0f;
    se->emoji_hsl[2] = -0.2f;
    se->merged_layer = layer_create(32, 128);
    se->image = image_create(32, 128);
    image_add_layer(se->image);
    font_init();
    gui_init();
}

void spinoedit_release(spinoedit_t *se)
{
    gui_release();
    image_delete(se->image);
    free(se->trx.prev_data);
    free(se->trx.frame);
}

static void spinoedit_stream(spinoedit_t *se)
{
	 
    int angle;
    const bool flip = true;
    int i, j, a, r;
    layer_t *layer;
    const uint8_t *img;
    int w = se->image->w;
    int h = se->image->h;
    const int buf_size = SPINO_BUF_SIZE(w, h);

    if (bluetooth_status(NULL) != SPINO_BT_CONNECTED) return;

    if (se->trx.state == SPINO_TRX_SENDING) {
        r = bluetooth_send(se->trx.frame + se->trx.frame_sent,
                           se->trx.frame_size - se->trx.frame_sent);
       
        
        assert(r >= 0);
        se->trx.frame_sent += r;
        assert(se->trx.frame_sent <= se->trx.frame_size);
        if (se->trx.frame_sent != se->trx.frame_size) return;
        se->trx.state = SPINO_TRX_WAITING_REPLY;
    }

    if (se->trx.state == SPINO_TRX_WAITING_REPLY) {
        r = bluetooth_receive();
        assert(r >= 0);
        if (r == 0) return;  // Not ready yet.
    }

    layer = layer_copy(se->merged_layer);
    layer_fix_center(layer);
    if (se->dithering)
        layer_dither(layer);
    img = layer->data;

    angle = se->device_angle / (M_PI * 2) * h;
    if (se->trx.buf_size != buf_size) {
        se->trx.buf_size = SPINO_BUF_SIZE(w, h);
        free(se->trx.buf);
        free(se->trx.prev_data);
        free(se->trx.frame);
        se->trx.prev_data = NULL;
        se->trx.prev_data_size = 0;
        se->trx.buf = calloc(1, buf_size);
        se->trx.frame = calloc(1, buf_size);
    }
    // Convert rgb image to uint8
    for (i = 0; i < h; i++)
    for (j = 0; j < w; j++) {
        a = i;
        a = (a + angle) % h;
        a = flip ? h - a - 1 : a;

      
        se->trx.buf[i * w + j] = (img[(a * w + w - j - 1) * 3 + 1] / 85) << 2 | // GREEN
                                (img[(a * w + w - j - 1) * 3 + 2] / 85) << 4 | // BLUE
                                (img[(a * w + w - j - 1) * 3 + 0] / 85) << 0; // RED


    }
    se->trx.frame_size = spino_make_frame(
            se->trx.buf, w * h,
            se->trx.prev_data, se->trx.prev_data_size,
            !(bool)se->trx.prev_data, false, 0,
            se->trx.frame, buf_size);

    if (!se->trx.prev_data) se->trx.prev_data = calloc(1, buf_size);
    memcpy(se->trx.prev_data, se->trx.buf, w * h);
    se->trx.prev_data_size = w * h;

    layer_delete(layer);
    se->trx.frame_sent = 0;
    se->trx.state = SPINO_TRX_SENDING;
}

void spinoedit_iter(spinoedit_t *se, inputs_t *inputs)
{
    se->screen_size[0] = inputs->window_size[0];
    se->screen_size[1] = inputs->window_size[1];
    gui_iter(inputs);
    spinoedit_set_hint_text(se, NULL);

    spinoedit_stream(se);
}

void spinoedit_render(spinoedit_t *spinoedit)
{
    GL(glViewport(0, 0, spinoedit->screen_size[0], spinoedit->screen_size[1]));
    GL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    GL(glClearColor(0, 0, 0, 1));
    GL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    gui_render();
}

void spinoedit_update_layers(spinoedit_t *se)
{
    layer_t *layer;
    layer_delete(se->merged_layer);
    se->merged_layer = layer_create(se->image->w, se->image->h);
    DL_FOREACH(se->image->layers, layer) {
        if (!layer->visible) continue;
        layer_merge(se->merged_layer, layer);
    }
}

void spinoedit_undo(spinoedit_t *se)
{
    image_undo(se->image);
}

void spinoedit_redo(spinoedit_t *se)
{
    image_redo(se->image);
}

void spinoedit_load(spinoedit_t *se, const char *path)
{
    image_t *img;
    img = file_load(path);
    if (!img) return;
    image_delete(se->image);
    se->image = img;
    spinoedit_update_layers(se);
}

void spinoedit_save(spinoedit_t *se, image_t *img, const char *path)
{
    file_save(img, path);
    spinoedit_update_layers(se);
}

void spinoedit_import(spinoedit_t *se, const char *path)
{
    file_import(se->image, path);
    spinoedit_update_layers(se);
}

void spinoedit_on_log(spinoedit_t *se, int level, const char *msg)
{
    if (level != GOX_LOG_ERROR) return;
    free(se->error_msg);
    se->error_msg = strdup(msg);
}

void spinoedit_set_hint_text(spinoedit_t *se, const char *msg, ...)
{
    va_list args;
    free(se->hint_text);
    se->hint_text = NULL;
    if (!msg) return;
    va_start(args, msg);
    vasprintf(&se->hint_text, msg, args);
    va_end(args);
}
