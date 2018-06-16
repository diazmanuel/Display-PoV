/* MIT License
 *
 * Copyright (c) 2017 spino.tech Guillaume Chereau & Raphael Seghier Guillaume Chereau & Raphael Seghier
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
#include <GLFW/glfw3.h>
#include <string.h>

#if DEBUG
#   define DEBUG_ONLY(x) x
#else
#   define DEBUG_ONLY(x)
#endif

static spinoedit_t  *g_spinoedit = NULL;
static GLFWwindow   *g_window = NULL;
static inputs_t     *g_inputs = NULL;

void on_scroll(GLFWwindow *win, double x, double y)
{
    g_inputs->mouse_wheel = y;
}

void on_char(GLFWwindow *win, unsigned int c)
{
    int i;
    if (c > 0 && c < 0x10000) {
        for (i = 0; i < ARRAY_SIZE(g_inputs->chars); i++) {
            if (!g_inputs->chars[i]) {
                g_inputs->chars[i] = c;
                break;
            }
        }
    }
}

static void loop_function(void)
{
    int fb_size[2], win_size[2];
    int i;
    double xpos, ypos;

    GL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    // The input struct gets all the values in framebuffer coordinates,
    // On retina display, this might not be the same as the window
    // size.
    glfwGetFramebufferSize(g_window, &fb_size[0], &fb_size[1]);
    glfwGetWindowSize(g_window, &win_size[0], &win_size[1]);
    g_inputs->window_size[0] = fb_size[0];
    g_inputs->window_size[1] = fb_size[1];

    for (i = 0; i <= GLFW_KEY_LAST; i++) {
        g_inputs->keys[i] = glfwGetKey(g_window, i) == GLFW_PRESS;
    }
    glfwGetCursorPos(g_window, &xpos, &ypos);
    g_inputs->mouse_pos[0] = xpos * fb_size[0] / win_size[0];
    g_inputs->mouse_pos[1] = ypos * fb_size[1] / win_size[1];
    g_inputs->mouse_down[0] =
        glfwGetMouseButton(g_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    g_inputs->mouse_down[1] =
        glfwGetMouseButton(g_window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS;
    g_inputs->mouse_down[2] =
        glfwGetMouseButton(g_window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
    spinoedit_iter(g_spinoedit, g_inputs);
    spinoedit_render(g_spinoedit);
    memset(g_inputs, 0, sizeof(*g_inputs));
    glfwSwapBuffers(g_window);

    glfwPollEvents();
}

void bluetooth_test(void);

int main(int argc, char **argv)
{
	remove("#descomprimido.txt");
	remove("#comprimido.txt");
	remove("#stream.txt");
    GLFWmonitor *monitor;
    const GLFWvidmode *mode;
    inputs_t inputs = {};
    const char *title = "Spino Brush " SPINO_VERSION_STR
                        DEBUG_ONLY(" (debug)");

    g_inputs = &inputs;
    g_spinoedit = calloc(1, sizeof(*g_spinoedit));
    glfwInit();

    glfwWindowHint(GLFW_SAMPLES, 2);
    monitor = glfwGetPrimaryMonitor();
    mode = glfwGetVideoMode(monitor);
    g_window = glfwCreateWindow(mode->width, mode->height, title, NULL, NULL);
    glfwMakeContextCurrent(g_window);
    glfwSetScrollCallback(g_window, on_scroll);
    glfwSetCharCallback(g_window, on_char);
    glfwSetInputMode(g_window, GLFW_STICKY_MOUSE_BUTTONS, false);
#ifdef WIN32
    glewInit();
#endif
    spinoedit_init(g_spinoedit);

    while (!glfwWindowShouldClose(g_window)) {
        loop_function();
    }
    spinoedit_release(g_spinoedit);
    glfwDestroyWindow(g_window);
    glfwTerminate();
    return 0;
}
