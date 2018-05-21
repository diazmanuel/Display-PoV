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

#ifndef _GNU_SOURCE
#   define _GNU_SOURCE
#endif

#ifndef NOMINMAX
#   define NOMINMAX
#endif

#include <assert.h>
#include <float.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>

#include "utlist.h"
#include "lz4.h"

#define SPINO_VERSION_CODE 2
#define SPINO_VERSION_STR "0.2.0"

// #### Set the DEBUG macro ####
#ifndef DEBUG
#   if !defined(NDEBUG)
#       define DEBUG 1
#   else
#       define DEBUG 0
#   endif
#endif

// #### DEFINED macro ##########
// DEFINE(NAME) returns 1 if NAME is defined to 1, 0 otherwise.
#define DEFINED(macro) DEFINED_(macro)
#define macrotest_1 ,
#define DEFINED_(value) DEFINED__(macrotest_##value)
#define DEFINED__(comma) DEFINED___(comma 1, 0)
#define DEFINED___(_, v, ...) v

// #### Logging macros #########

enum {
    GOX_LOG_VERBOSE = 2,
    GOX_LOG_DEBUG   = 3,
    GOX_LOG_INFO    = 4,
    GOX_LOG_WARN    = 5,
    GOX_LOG_ERROR   = 6,
};

#ifndef LOG_LEVEL
#   if DEBUG
#       define LOG_LEVEL GOX_LOG_DEBUG
#   else
#       define LOG_LEVEL GOX_LOG_INFO
#   endif
#endif

#define LOG(level, msg, ...) do { \
    if (level >= LOG_LEVEL) \
        dolog(level, msg, __func__, __FILE__, __LINE__, ##__VA_ARGS__); \
} while(0)

#define LOG_V(msg, ...) LOG(GOX_LOG_VERBOSE, msg, ##__VA_ARGS__)
#define LOG_D(msg, ...) LOG(GOX_LOG_DEBUG,   msg, ##__VA_ARGS__)
#define LOG_I(msg, ...) LOG(GOX_LOG_INFO,    msg, ##__VA_ARGS__)
#define LOG_W(msg, ...) LOG(GOX_LOG_WARN,    msg, ##__VA_ARGS__)
#define LOG_E(msg, ...) LOG(GOX_LOG_ERROR,   msg, ##__VA_ARGS__)

// CHECK is similar to an assert, but the condition is tested even in release
// mode.
#if DEBUG
    #define CHECK(c) assert(c)
#else
    #define CHECK(c) do { \
        if (!(c)) { \
            LOG_E("Error %s %s %d", __func__,  __FILE__, __LINE__); \
            exit(-1); \
        } \
    } while (0)
#endif

// I redefine asprintf so that if the function fails, we just crash the
// application.  I don't see how we can recover from an asprintf fails
// anyway.
#define asprintf(...) CHECK(asprintf(__VA_ARGS__) != -1)
#define vasprintf(...) CHECK(vasprintf(__VA_ARGS__) != -1)

// #### Utils ##################

// Used internally by the LOG macro
void dolog(int level, const char *msg,
           const char *func, const char *file, int line, ...);
int check_gl_errors(const char *file, int line);
int create_program(const char *vertex_shader, const char *fragment_shader,
                   const char *include);
void delete_program(int prog);
uint8_t *img_read(const char *path, int *width, int *height, int *bpp);
uint8_t *img_read_from_mem(const char *data, int size,
                           int *w, int *h, int *bpp);
void img_write(const uint8_t *img, int w, int h, int bpp, const char *path);
int decode_85(char *dst, const char *buffer, int len);

// #### Include OpenGL #########
#define GL_GLEXT_PROTOTYPES
#ifdef WIN32
#    include <windows.h>
#    include "GL/glew.h"
#endif
#ifdef __APPLE__
#   include "TargetConditionals.h"
#   if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
#       define GLES2 1
#       include <OPenGLES/ES2/gl.h>
#       include <OpenGLES/ES2/glext.h>
#   else
#       include <OpenGL/gl.h>
#   endif
#else
#   ifdef GLES2
#       include <GLES2/gl2.h>
#       include <GLES2/gl2ext.h>
#   else
#       include <GL/gl.h>
#   endif
#endif
// #############################



// #### GL macro ###############
#if DEBUG
#  define GL(line) ({                                                   \
       line;                                                            \
       if (check_gl_errors(__FILE__, __LINE__)) assert(false);          \
   })
#else
#  define GL(line) line
#endif
// #############################

// ### Some useful inline functions / macros.

#define DR2D (180 / M_PI)
#define DD2R (M_PI / 180)

#define ARRAY_SIZE(x) ((int)(sizeof(x) / sizeof((x)[0])))
#define SWAP(x0, x) {typeof(x0) tmp = x0; x0 = x; x = tmp;}

#define min(a, b) ({ \
      __typeof__ (a) _a = (a); \
      __typeof__ (b) _b = (b); \
      _a < _b ? _a : _b; \
      })

#define max(a, b) ({ \
      __typeof__ (a) _a = (a); \
      __typeof__ (b) _b = (b); \
      _a > _b ? _a : _b; \
      })

#define max3(x, y, z) (max((x), max((y), (z))))
#define min3(x, y, z) (min((x), min((y), (z))))

#define clamp(x, a, b) (min(max(x, a), b))

#define sign(x) ({ \
      __typeof__ (x) _x = (x); \
      (_x > 0) ? +1 : (_x < 0)? -1 : 0; \
      })

static inline float smoothstep(float edge0, float edge1, float x)
{
    x = clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    return x * x * (3.0f - 2.0f * x);
}

static inline float mix(float x, float y, float t)
{
    return (1.0 - t) * x + t * y;
}

// #### System #################
void sys_log(const char *msg);
const char *sys_get_clipboard_text(void);
void sys_set_clipboard_text(const char *text);
const char *get_data_path(const char *file);

// #### Storage ################
void storage_init(void);
void storage_set(const char *key, const char *value);
const char *storage_get(const char *key, const char *default_value);
void storage_set_int(const char *key, int value);
int storage_get_int(const char *key, int default_value);
int storage_inc_int(const char *key);

// #### Emojies

typedef struct emoji emoji_t;
struct emoji {
    const char      *name;
    const char      *shortname;
    const char      *category;
    const char      *data;
    int             data_size;
    int             w, h;
    unsigned int    texture;    // OpenGL Texture.
};

const char** emoji_get_categories(int *nb);
void emoji_iter(const char *category, int (*f)(emoji_t *emoji, int i));
int emoji_get_texture(emoji_t *emoji);
uint8_t *emoji_get_img(emoji_t *emoji);

// Key id, same as GLFW for convenience.
enum {
    KEY_ESCAPE      = 256,
    KEY_ENTER       = 257,
    KEY_TAB         = 258,
    KEY_BACKSPACE   = 259,
    KEY_DELETE      = 261,
    KEY_RIGHT       = 262,
    KEY_LEFT        = 263,
    KEY_DOWN        = 264,
    KEY_UP          = 265,
    KEY_PAGE_UP     = 266,
    KEY_PAGE_DOWN   = 267,
    KEY_HOME        = 268,
    KEY_END         = 269,
    KEY_SHIFT       = 340,
    KEY_CONTROL     = 341,
};

typedef struct inputs
{
    int         window_size[2];
    bool        keys[512]; // Table of all the pressed keys.
    uint32_t    chars[16];
    float       mouse_pos[2];
    bool        mouse_down[3];
    float       mouse_wheel;
} inputs_t;

// ##### Image effects
void effect_ordered_dithering(uint8_t *img, int w, int h);
void effect_hsl(uint8_t *img, int w, int h, int bpp,
                float hue, float sat, float light);

typedef struct layer layer_t;
struct layer {
    layer_t     *next, *prev;
    char        name[16];
    int         w, h;
    uint8_t     *data;      // RGB data.
    bool        visible;
    int         *ref;       // To implement copy on write.

    int         data_i;     // Only used when we save.
};

layer_t *layer_create(int w, int h);
void layer_clear(layer_t *layer);
void layer_merge(layer_t *layer, const layer_t *other);
layer_t *layer_copy(layer_t *other);
void layer_delete(layer_t* layer);
void layer_paint(layer_t *layer, int x, int y, uint8_t color[3],
                 float radius, float smoothness, bool spherical);
void layer_paint_line(layer_t *layer, int x1, int y1, int x2, int y2,
                      uint8_t color[3], float radius, float smoothness,
                      bool spherical);
void layer_paint_text(layer_t *layer, const char *text,
                      const uint8_t color[3]);
void layer_paint_emoji(layer_t *layer, emoji_t *emoji);
void layer_set_data(layer_t *layer, uint8_t *data, int w, int h, int bpp,
                    int x, int y);
void layer_set_rdata(layer_t *layer, uint8_t *data, int w, int h, int bpp,
                     float scale);
void layer_set_data_from_layer(layer_t *layer, const layer_t *other);
void layer_get_color_at(const layer_t *layer, int x, int y, uint8_t out[3]);
void layer_translate(layer_t *layer, int x, int y);
void layer_fix_center(layer_t *layer);
void layer_dither(layer_t *layer);

typedef struct image image_t;
struct image
{
    int     w, h;
    layer_t *layers;
    layer_t *layer;     // Current active layer.
    char    *path;

    image_t *history;
    image_t *history_next, *history_prev, *history_current;
};

image_t *image_create(int w, int h);
void image_delete(image_t *img);
void image_history_push(image_t *img);
void image_undo(image_t *img);
void image_redo(image_t *img);
layer_t *image_add_layer(image_t *img);
void image_delete_layer(image_t *img, layer_t *layer);
layer_t *image_duplicate_layer(image_t *img, layer_t *layer);
void image_move_layer(image_t *img, layer_t *layer, int ofs);
void image_merge_visible_layers(image_t *img);
// Remove duplicated layers data.
void image_compress(image_t *img);

enum {
    SPINO_TRX_IDLE,
    SPINO_TRX_SENDING,
    SPINO_TRX_WAITING_REPLY,
};

typedef struct spinoedit spinoedit_t;
struct spinoedit {
    int         screen_size[2];
    char        *help_text;  // Seen at the bottom of the screen.
    char        *hint_text;  // Seen at the bottom of the screen.
    char        *error_msg;  // Set in case of error.

    uint8_t     color[3];    // Selected color.
    int         size;        // Brush size in pixels.
    bool        smooth;      // Brush smooth or not.
    bool        dithering;   // Apply dithering or not when sending to spino.
    // XXX: redo zoom properly.
    float       zoom;        // size of pixels.
    float       zoom2;       // size of pixels.
    int         state;       // tool state.
    float       angle;       // Rotation angle (rad).
    float       device_angle;// Rotation angle on the device (rad).
    int         symmetry;    // Symmetry effect.

    // hsl effect applied to the emojis.
    float       emoji_hsl[3];

    image_t     *image;
    layer_t     *merged_layer; // All the image layers merged.

    // Transmition buffers.
    struct {
        int         buf_size;   // Max size for all buffers.

        uint8_t     *buf;       // Tmp buffer.
        uint8_t     *frame;
        uint8_t     *prev_data;

        int         frame_size;
        int         prev_data_size;
        int         frame_sent;     // Number of bytes already sent.
        int         state;          // SPINO_TRX_ value
    } trx;
};

void spinoedit_init(spinoedit_t *spinoedit);
void spinoedit_release(spinoedit_t *spinoedit);
void spinoedit_iter(spinoedit_t *spinoedit, inputs_t *inputs);
void spinoedit_render(spinoedit_t *spinoedit);
spinoedit_t *spinoedit(void);
void spinoedit_update_layers(spinoedit_t *se);
void spinoedit_undo(spinoedit_t *se);
void spinoedit_redo(spinoedit_t *se);
void spinoedit_on_log(spinoedit_t *se, int level, const char *msg);

void spinoedit_load(spinoedit_t *se, const char *path);
void spinoedit_save(spinoedit_t *se, image_t *img, const char *path);
void spinoedit_import(spinoedit_t *se, const char *path);

void spinoedit_set_hint_text(spinoedit_t *se, const char *msg, ...);

// #### Gui ####################

void gui_init(void);
void gui_release(void);
void gui_iter(const inputs_t *inputs);
void gui_render(void);

// #### Load / Save file #######
image_t * file_load(const char *path);
void file_save(image_t *img, const char *path);
void file_import(image_t *img, const char *path);

// #### Dialogs ################
enum {
    DIALOG_FLAG_SAVE    = 1 << 0,
    DIALOG_FLAG_OPEN    = 1 << 1,
    DIALOG_FLAG_DIR     = 1 << 2,
};

// out will be set to a newly allocated string of NULL.
bool dialog_open(int flags, const char *filter, char **out);

extern const uint8_t PALETTE[64][3];

// #### Font support ##########
void font_init(void);
// return an image of the text.
uint8_t *font_print(const char *msg, int *w, int *h);

// #### Bluetooth support #####

enum {
    SPINO_BT_IDLE = 0,
    SPINO_BT_LOADING,
    SPINO_BT_NOT_AVAILABLE,
    SPINO_BT_ERROR,
    SPINO_BT_LOADED,
    SPINO_BT_CONNECTED,
};

void bluetooth_init(void);
void bluetooth_refresh_list(void);
void bluetooth_remember(const char *addr, const char *name);
int bluetooth_status(char *msg);
void bluetooth_get_devices(void (*f)(const char *addr, const char *name));
void bluetooth_connect(const char *addr);
int bluetooth_send(const void *data, int size);
int bluetooth_receive(void);

void bluetooth_set_connect_callback(int (*f)(void));
void bluetooth_set_send_callback(int (*f)(const void *data, int size));
// Signal connection (0) or data received (1)
void bluetooth_signal(int type, int data);

// #### Spino data compression ####

#define SPINO_HEADER_SIZE 10

// Macro that defines the minimum size of the data buffer for a given
// screen size.
#define SPINO_BUF_SIZE(w, h) \
    (LZ4_COMPRESSBOUND((w) * (h)) + SPINO_HEADER_SIZE)

int spino_make_frame(const uint8_t *data, int size,
                     const uint8_t *prev_data, int prev_data_size,
                     bool indep, bool line_mode, int pwm,
                     uint8_t *out, int out_max_size);

