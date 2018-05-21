/* MIT License
 *
 * Copyright (c) 2017 spino.tech
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

# if TARGET_OS_MAC == 1

#include <AppKit/AppKit.h>

bool dialog_open(int flags, const char *filters, char **out)
{
    NSURL *url;
    const char *utf8_path;
    NSSavePanel *panel;
    NSOpenPanel *open_panel;
    NSMutableArray *types_array;
    // XXX: I don't know about memory management with cococa, need to check
    // if I leak memory here.

    *out = NULL;
    if (flags & DIALOG_FLAG_OPEN) {
        panel = open_panel = [NSOpenPanel openPanel];
    } else {
        panel = [NSSavePanel savePanel];
    }

    if (flags & DIALOG_FLAG_DIR) {
        [open_panel setCanChooseDirectories:YES];
        [open_panel setCanChooseFiles:NO];
    }

    if (filters) {
        types_array = [NSMutableArray array];
        while (*filters) {
            filters += strlen(filters) + 1; // skip the name
            assert(strncmp(filters, "*.", 2) == 0);
            filters += 2; // Skip the "*."
            [types_array addObject:[NSString stringWithUTF8String: filters]];
            filters += strlen(filters) + 1;
        }
        [panel setAllowedFileTypes:types_array];
    }

    if ( [panel runModal] == NSModalResponseOK ) {
        url = [panel URL];
        utf8_path = [[url path] UTF8String];
        *out = strdup(utf8_path);
    }

    return *out != NULL;
}

# endif
