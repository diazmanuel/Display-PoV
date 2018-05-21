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

image_t *image_create(int w, int h)
{
    image_t *img = calloc(1, sizeof(*img));
    img->w = w;
    img->h = h;
    return img;
}

static void image_delete_history(image_t *img)
{
    image_t *img2, *tmp;
    DL_FOREACH_SAFE2(img->history, img2, tmp, history_next) {
        DL_DELETE2(img->history, img2, history_prev, history_next);
        assert(img2 != img);
        image_delete(img2);
    }
    img->history = NULL;
    img->history_next = NULL;
    img->history_prev = NULL;
    img->history_current = NULL;
}

void image_delete(image_t *img)
{
    layer_t *layer, *tmp;

    image_delete_history(img);
    DL_FOREACH_SAFE(img->layers, layer, tmp) {
        DL_DELETE(img->layers, layer);
        layer_delete(layer);
    }
    free(img->path);
    free(img);
}

image_t *image_copy(image_t *other)
{
    image_t *img;
    layer_t *layer, *other_layer;
    img = calloc(1, sizeof(*img));
    img->path = other->path ? strdup(other->path) : NULL;
    img->w = other->w;
    img->h = other->h;
    DL_FOREACH(other->layers, other_layer) {
        layer = layer_copy(other_layer);
        DL_APPEND(img->layers, layer);
        if (other_layer == other->layer)
            img->layer = layer;
    }
    assert(img->layer);
    return img;
}

void image_set(image_t *img, image_t *other)
{
    layer_t *layer, *tmp, *other_layer;
    DL_FOREACH_SAFE(img->layers, layer, tmp) {
        DL_DELETE(img->layers, layer);
        layer_delete(layer);
    }
    DL_FOREACH(other->layers, other_layer) {
        layer = layer_copy(other_layer);
        DL_APPEND(img->layers, layer);
        if (other_layer == other->layer)
            img->layer = layer;
    }
}

void image_history_push(image_t *img)
{
    image_t *snap = image_copy(img);
    image_t *tmp;

    // Discard previous undo.
    // XXX: also need to delete the images!!
    while (img->history_current) {
        tmp = img->history_current;
        img->history_current = tmp->history_next;
        DL_DELETE2(img->history, tmp, history_prev, history_next);
    }

    DL_APPEND2(img->history, snap, history_prev, history_next);
    img->history_current = NULL;
}

void image_undo(image_t *img)
{
    if (img->history_current == img->history) {
        LOG_D("No more undo");
        return;
    }
    if (!img->history_current) {
        image_t *snap = image_copy(img);
        DL_APPEND2(img->history, snap, history_prev, history_next);
        img->history_current = img->history->history_prev;
    }

    image_set(img, img->history_current->history_prev);
    img->history_current = img->history_current->history_prev;
    spinoedit_update_layers(spinoedit());
}

void image_redo(image_t *img)
{
    if (!img->history_current || !img->history_current->history_next) {
        LOG_D("No more redo");
        return;
    }
    img->history_current = img->history_current->history_next;
    image_set(img, img->history_current);
    spinoedit_update_layers(spinoedit());
}

layer_t *image_add_layer(image_t *img)
{
    layer_t *layer;
    layer = layer_create(img->w, img->h);
    DL_APPEND(img->layers, layer);
    img->layer = layer;
    spinoedit_update_layers(spinoedit());
    return layer;
}

void image_delete_layer(image_t *img, layer_t *layer)
{
    DL_DELETE(img->layers, layer);
    if (layer == img->layer) img->layer = NULL;
    layer_delete(layer);
    if (img->layers == NULL) {
        layer = layer_create(img->w, img->h);
        DL_APPEND(img->layers, layer);
    }
    if (!img->layer) img->layer = img->layers->prev;
    spinoedit_update_layers(spinoedit());
}

layer_t *image_duplicate_layer(image_t *img, layer_t *other)
{
    layer_t *layer;
    layer = layer_copy(other);
    layer->visible = true;
    DL_APPEND(img->layers, layer);
    img->layer = layer;
    return layer;
}

void image_move_layer(image_t *img, layer_t *layer, int d)
{
    assert(d == -1 || d == +1);
    layer_t *other = NULL;
    if (d == -1) {
        other = layer->next;
        SWAP(other, layer);
    } else if (layer != img->layers) {
        other = layer->prev;
    }
    if (!other || !layer) return;
    DL_DELETE(img->layers, layer);
    DL_PREPEND_ELEM(img->layers, other, layer);
    spinoedit_update_layers(spinoedit());
}

void image_merge_visible_layers(image_t *img)
{
    layer_t *layer, *last = NULL;
    DL_FOREACH(img->layers, layer) {
        if (!layer->visible) continue;
        if (last) {
            layer_merge(layer, last);
            DL_DELETE(img->layers, last);
            layer_delete(last);
        }
        last = layer;
    }
    if (last) img->layer = last;
    spinoedit_update_layers(spinoedit());
}

void image_compress(image_t *img)
{
    int size = img->w * img->h * 3;
    layer_t *layer, *other;
    DL_FOREACH(img->layers, layer) {
        for (other = img->layers; other != layer; other = other->next) {
            if (    (layer->data != other->data) &&
                    memcmp(layer->data, other->data, size) == 0) {
                layer_set_data_from_layer(layer, other);
            }
        }
    }
}
