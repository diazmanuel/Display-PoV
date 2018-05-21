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

void sys_log(const char *msg)
{
    printf("%s\n", msg);
    fflush(stdout);
}

#ifdef __linux__

const char *get_data_path(const char *file)
{
    return file;
}

#include <gtk/gtk.h>

const char *sys_get_clipboard_text(void)
{
    GtkClipboard *cb;
    static gchar *text = NULL;
    gtk_init_check(NULL, NULL);
    g_free(text);
    cb = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
    text = gtk_clipboard_wait_for_text(cb);
    return text;
}

void sys_set_clipboard_text(const char *text)
{
    GtkClipboard *cb;
    gtk_init_check(NULL, NULL);
    cb = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
    gtk_clipboard_set_can_store(cb, NULL, 0);
    gtk_clipboard_set_text(cb, text, -1);
    gtk_clipboard_store(cb);
}

#endif
