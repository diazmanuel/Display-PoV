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

extern "C" {
#include "spino.h"
}
#include "imgui.h"

namespace ImGui {
    bool GoxTab(const char *label, bool *v);
};

static inline ImVec4 IMHEXCOLOR(uint32_t v)
{
    int r, g, b, a;
    r = (v >> 24) & 0xff;
    g = (v >> 16) & 0xff;
    b = (v >>  8) & 0xff;
    a = (v >>  0) & 0xff;
    return ImVec4(r / 255.0, g / 255.0, b / 255.0, a / 255.0);
}

static const char *VSHADER =
    "                                                               \n"
    "attribute vec3 a_pos;                                          \n"
    "attribute vec2 a_tex_pos;                                      \n"
    "attribute vec4 a_color;                                        \n"
    "                                                               \n"
    "uniform mat4 u_proj_mat;                                       \n"
    "                                                               \n"
    "varying vec2 v_tex_pos;                                        \n"
    "varying vec4 v_color;                                          \n"
    "                                                               \n"
    "void main()                                                    \n"
    "{                                                              \n"
    "    gl_Position = u_proj_mat * vec4(a_pos, 1.0);               \n"
    "    v_tex_pos = a_tex_pos;                                     \n"
    "    v_color = a_color;                                         \n"
    "}                                                              \n"
;

static const char *FSHADER =
    "                                                               \n"
    "#ifdef GL_ES                                                   \n"
    "precision mediump float;                                       \n"
    "#endif                                                         \n"
    "                                                               \n"
    "uniform sampler2D u_tex;                                       \n"
    "                                                               \n"
    "varying vec2 v_tex_pos;                                        \n"
    "varying vec4 v_color;                                          \n"
    "                                                               \n"
    "void main()                                                    \n"
    "{                                                              \n"
    "    gl_FragColor = v_color * texture2D(u_tex, v_tex_pos);      \n"
    "}                                                              \n"
;

typedef struct {
    GLuint prog;

    GLuint a_pos_l;
    GLuint a_tex_pos_l;
    GLuint a_color_l;

    GLuint u_tex_l;
    GLuint u_proj_mat_l;
} prog_t;

typedef struct gui_t {
    prog_t  prog;
    GLuint  array_buffer;
    GLuint  index_buffer;

    char prog_path[1024];       // "\0" if no loaded prog.
    char prog_buff[64 * 1024];  // XXX: make it dynamic?
    bool prog_export_animation;
    char prog_export_animation_path[1024];
} gui_t;

static gui_t *gui = NULL;

static void init_prog(prog_t *p)
{
    p->prog = create_program(VSHADER, FSHADER, NULL);
    GL(glUseProgram(p->prog));
#define UNIFORM(x) p->x##_l = glGetUniformLocation(p->prog, #x);
#define ATTRIB(x) p->x##_l = glGetAttribLocation(p->prog, #x)
    UNIFORM(u_proj_mat);
    UNIFORM(u_tex);
    ATTRIB(a_pos);
    ATTRIB(a_tex_pos);
    ATTRIB(a_color);
#undef ATTRIB
#undef UNIFORM
    GL(glUniform1i(p->u_tex_l, 0));
}


static void render_prepare_context(void)
{
    #define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))
    // Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled
    GL(glEnable(GL_BLEND));
    GL(glBlendEquation(GL_FUNC_ADD));
    GL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    GL(glDisable(GL_CULL_FACE));
    GL(glDisable(GL_DEPTH_TEST));
    GL(glEnable(GL_SCISSOR_TEST));
    GL(glActiveTexture(GL_TEXTURE0));

    // Setup orthographic projection matrix
    const float width = ImGui::GetIO().DisplaySize.x;
    const float height = ImGui::GetIO().DisplaySize.y;
    const float ortho_projection[4][4] =
    {
        { 2.0f/width,	0.0f,			0.0f,		0.0f },
        { 0.0f,			2.0f/-height,	0.0f,		0.0f },
        { 0.0f,			0.0f,			-1.0f,		0.0f },
        { -1.0f,		1.0f,			0.0f,		1.0f },
    };
    GL(glUseProgram(gui->prog.prog));
    GL(glUniformMatrix4fv(gui->prog.u_proj_mat_l, 1, 0, &ortho_projection[0][0]));

    GL(glBindBuffer(GL_ARRAY_BUFFER, gui->array_buffer));
    GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gui->index_buffer));
    // This could probably be done only at init time.
    GL(glEnableVertexAttribArray(gui->prog.a_pos_l));
    GL(glEnableVertexAttribArray(gui->prog.a_tex_pos_l));
    GL(glEnableVertexAttribArray(gui->prog.a_color_l));
    GL(glVertexAttribPointer(gui->prog.a_pos_l, 2, GL_FLOAT, false,
                             sizeof(ImDrawVert),
                             (void*)OFFSETOF(ImDrawVert, pos)));
    GL(glVertexAttribPointer(gui->prog.a_tex_pos_l, 2, GL_FLOAT, false,
                             sizeof(ImDrawVert),
                             (void*)OFFSETOF(ImDrawVert, uv)));
    GL(glVertexAttribPointer(gui->prog.a_color_l, 4, GL_UNSIGNED_BYTE,
                             true, sizeof(ImDrawVert),
                             (void*)OFFSETOF(ImDrawVert, col)));
    #undef OFFSETOF
}

static void ImImpl_RenderDrawLists(ImDrawData* draw_data)
{
    const float height = ImGui::GetIO().DisplaySize.y;
    render_prepare_context();
    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        const ImDrawIdx* idx_buffer_offset = 0;

        GL(glBufferData(GL_ARRAY_BUFFER,
                    (GLsizeiptr)cmd_list->VtxBuffer.size() * sizeof(ImDrawVert),
                    (GLvoid*)&cmd_list->VtxBuffer.front(), GL_DYNAMIC_DRAW));

        GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                    (GLsizeiptr)cmd_list->IdxBuffer.size() * sizeof(ImDrawIdx),
                    (GLvoid*)&cmd_list->IdxBuffer.front(), GL_DYNAMIC_DRAW));

        for (const ImDrawCmd* pcmd = cmd_list->CmdBuffer.begin(); pcmd != cmd_list->CmdBuffer.end(); pcmd++)
        {
            if (pcmd->UserCallback)
            {
                pcmd->UserCallback(cmd_list, pcmd);
                render_prepare_context(); // Restore context.
            }
            else
            {
                GL(glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId));
                GL(glScissor((int)pcmd->ClipRect.x, (int)(height - pcmd->ClipRect.w),
                             (int)(pcmd->ClipRect.z - pcmd->ClipRect.x),
                             (int)(pcmd->ClipRect.w - pcmd->ClipRect.y)));
                GL(glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount,
                                  GL_UNSIGNED_INT, idx_buffer_offset));
            }
            idx_buffer_offset += pcmd->ElemCount;
        }
    }
    GL(glDisable(GL_SCISSOR_TEST));
}

static void load_fonts_texture()
{
    ImGuiIO& io = ImGui::GetIO();

    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

    GLuint tex_id;
    glGenTextures(1, &tex_id);
    glBindTexture(GL_TEXTURE_2D, tex_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    io.Fonts->TexID = (void *)(intptr_t)tex_id;
}

static void init_ImGui()
{
    ImGuiIO& io = ImGui::GetIO();
    io.DeltaTime = 1.0f/60.0f;

    io.KeyMap[ImGuiKey_Tab]         = KEY_TAB;
    io.KeyMap[ImGuiKey_LeftArrow]   = KEY_LEFT;
    io.KeyMap[ImGuiKey_RightArrow]  = KEY_RIGHT;
    io.KeyMap[ImGuiKey_UpArrow]     = KEY_UP;
    io.KeyMap[ImGuiKey_DownArrow]   = KEY_DOWN;
    io.KeyMap[ImGuiKey_PageUp]      = KEY_PAGE_UP;
    io.KeyMap[ImGuiKey_PageDown]    = KEY_PAGE_DOWN;
    io.KeyMap[ImGuiKey_Home]        = KEY_HOME;
    io.KeyMap[ImGuiKey_End]         = KEY_END;
    io.KeyMap[ImGuiKey_Delete]      = KEY_DELETE;
    io.KeyMap[ImGuiKey_Backspace]   = KEY_BACKSPACE;
    io.KeyMap[ImGuiKey_Enter]       = KEY_ENTER;
    io.KeyMap[ImGuiKey_Escape]      = KEY_ESCAPE;
    io.KeyMap[ImGuiKey_A]           = 'A';
    io.KeyMap[ImGuiKey_C]           = 'C';
    io.KeyMap[ImGuiKey_V]           = 'V';
    io.KeyMap[ImGuiKey_X]           = 'X';
    io.KeyMap[ImGuiKey_Y]           = 'Y';
    io.KeyMap[ImGuiKey_Z]           = 'Z';

    if (DEFINED(__linux__)) {
        io.SetClipboardTextFn = sys_set_clipboard_text;
        io.GetClipboardTextFn = sys_get_clipboard_text;
    }

    io.RenderDrawListsFn = ImImpl_RenderDrawLists;
    load_fonts_texture();

    ImGuiStyle& style = ImGui::GetStyle();
    style.FrameRounding = 0;
    style.WindowRounding = 0;
    style.ItemSpacing = ImVec2(4, 4);
    style.AntiAliasedShapes = false;
    style.Colors[ImGuiCol_WindowBg] = IMHEXCOLOR(0x202020FF);
    style.Colors[ImGuiCol_Header] = style.Colors[ImGuiCol_WindowBg];
    style.Colors[ImGuiCol_Text] = IMHEXCOLOR(0xD0D0D0FF);
    style.Colors[ImGuiCol_Button] = IMHEXCOLOR(0x727272FF);
    style.Colors[ImGuiCol_ButtonActive] = IMHEXCOLOR(0x6666CCFF);
    style.Colors[ImGuiCol_ButtonHovered] = IMHEXCOLOR(0x6666FFFF);
}

void gui_init(void)
{
    gui = (gui_t*)calloc(1, sizeof(*gui));
    init_prog(&gui->prog);
    GL(glGenBuffers(1, &gui->array_buffer));
    GL(glGenBuffers(1, &gui->index_buffer));
    init_ImGui();
}

void gui_release(void)
{
    ImGui::Shutdown();
}


static void tool_panel(void)
{
    int v;
    spinoedit_t *se = spinoedit();

    v = round(se->angle * DR2D);
    if (ImGui::InputInt("Angle", &v, 10)) {
        while (v < 0) v += 360;
        v = v % 360;
        se->angle = v * DD2R;
    }

    if (ImGui::InputInt("size", &se->size))
        se->size = clamp(se->size, 1, 32);
    ImGui::Checkbox("smooth", &se->smooth);
}

static void palette_panel(void)
{
    int i, r, g, b;
    bool current;
    ImVec4 c;
    ImVec2 ra, rb;
    spinoedit_t *se = spinoedit();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    for (i = 0; i < 64; i++) {
        r = PALETTE[i][0];
        g = PALETTE[i][1];
        b = PALETTE[i][2];
        c = ImVec4(r / 255., g / 255., b / 255., 1);
        current = (r == se->color[0] &&
                   g == se->color[1] &&
                   b == se->color[2]);
        ImGui::PushID(i);
        if (current) {
            ra = ImGui::GetCursorScreenPos();
            rb = ImVec2(ra.x + 20, ra.y + 15);
            ra.x -= 2; ra.y -= 2;
            draw_list->AddRect(ra, rb, 0xFFFFFFFF, 0, 0, 2);
        }
        if (ImGui::ColorButton(c, true, current)) {
            se->color[0] = r;
            se->color[1] = g;
            se->color[2] = b;
        }
        ImGui::PopID();
        if (i % 8 != 7)
            ImGui::SameLine();
    }
}

static void toggle_layer_only_visible(spinoedit_t *se, layer_t *layer)
{
    layer_t *other;
    bool others_all_invisible = true;
    DL_FOREACH(se->image->layers, other) {
        if (other == layer) continue;
        if (other->visible) {
            others_all_invisible = false;
            break;
        }
    }
    DL_FOREACH(se->image->layers, other)
        other->visible = others_all_invisible;
    layer->visible = true;
}

static void layers_panel(void)
{
    spinoedit_t *se = spinoedit();
    layer_t *layer;
    ImGui::PushID("layers_planel");
    int i = 0;
    bool current;
    DL_FOREACH(se->image->layers, layer) {
        ImGui::PushID(i);
        current = se->image->layer == layer;
        if (ImGui::Selectable(current ? "*" : " ", &current, 0,
                              ImVec2(12, 12))) {
            if (current) {
                se->image->layer = layer;
                layer->visible = true;
                spinoedit_update_layers(se);
            }
        }
        if (ImGui::IsItemHovered())
            spinoedit_set_hint_text(se, "Set active layer");

        ImGui::SameLine();
        if (ImGui::Selectable(layer->visible ? "v##v" : " ##v",
                    &layer->visible, 0, ImVec2(12, 12))) {
            if (ImGui::IsKeyDown(KEY_SHIFT))
                toggle_layer_only_visible(se, layer);
            spinoedit_update_layers(se);
        }
        if (ImGui::IsItemHovered()) {
            spinoedit_set_hint_text(se,
                    "Toggle layer visibility. "
                    "Shift click to unselect all other layers");
        }
        ImGui::SameLine();
        ImGui::InputText("##name", layer->name, sizeof(layer->name));
        i++;
        ImGui::PopID();
    }
    if (ImGui::Button("New layer")) {
        image_history_push(se->image);
        image_add_layer(se->image);
    }
    if (ImGui::Button("Duplicate layer")) {
        image_history_push(se->image);
        image_duplicate_layer(se->image, se->image->layer);
    }
    if (ImGui::Button("Delete layer")) {
        image_history_push(se->image);
        image_delete_layer(se->image, se->image->layer);
    }
    if (ImGui::Button("^")) {
        image_history_push(se->image);
        image_move_layer(se->image, se->image->layer, +1);
    }
    ImGui::SameLine();
    if (ImGui::Button("v")) {
        image_history_push(se->image);
        image_move_layer(se->image, se->image->layer, -1);
    }
    if (ImGui::Button("Merge visible layers")) {
        image_history_push(se->image);
        image_merge_visible_layers(se->image);
    }
    ImGui::PopID();
}

static void effects_panel(void)
{
    int i;
    spinoedit_t *se = spinoedit();
    ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue;
    i = 0;
    if (ImGui::InputInt("Move X", &i, 1, -1, flags)) {
        image_history_push(se->image);
        layer_translate(se->image->layer, -i, 0);
        spinoedit_update_layers(se);
    }
    i = 0;
    if (ImGui::InputInt("Move Y", &i, 1, -1, flags)) {
        image_history_push(se->image);
        layer_translate(se->image->layer, 0, i);
        spinoedit_update_layers(se);
    }
    if (ImGui::InputInt("Sym", &se->symmetry))
        se->symmetry = clamp(se->symmetry, 1, 8);
}

static int on_emoji(emoji_t *emoji, int i)
{
    int texture;
    layer_t *layer;
    spinoedit_t *se = spinoedit();
    texture = emoji_get_texture(emoji);
    if (ImGui::ImageButton((void*)(intptr_t)texture, ImVec2(32, 32))) {
        image_history_push(se->image);
        layer = se->image->layer;
        layer_clear(layer);
        layer_paint_emoji(layer, emoji);
        snprintf(layer->name, ARRAY_SIZE(layer->name) - 1,
                 "%s", emoji->shortname);
        spinoedit_update_layers(se);
    }
    if (i % 4 != 3) ImGui::SameLine();
    return 0;
}

static void emojis_panel(void)
{
    const char **categories;
    static int cati = 0;
    int nb;
    int v;
    spinoedit_t *se = spinoedit();

    v = round(se->emoji_hsl[1] * 10);
    if (ImGui::InputInt("sat", &v))
        se->emoji_hsl[1] = clamp(v / 10., -1, +1);

    v = round(se->emoji_hsl[2] * 10);
    if (ImGui::InputInt("light", &v))
        se->emoji_hsl[2] = clamp(v / 10., -1, +1);

    categories = emoji_get_categories(&nb);
    ImGui::Combo("Category", &cati, categories, nb);
    emoji_iter(categories[cati], on_emoji);
}

static void text_panel(void)
{
    spinoedit_t *se = spinoedit();
    static char text[16];

    layer_t *layer;
    ImGui::InputText("text", text, sizeof(text));
    if (ImGui::Button("create text")) {
        image_history_push(se->image);
        layer = image_add_layer(se->image);
        layer_paint_text(layer, text, se->color);
        sprintf(layer->name, "%s", text);
        spinoedit_update_layers(se);
    }
}

static void on_bt_device(const char *addr, const char *name)
{
    if (ImGui::Button(name)) {
        bluetooth_connect(addr);
        bluetooth_remember(addr, name);
    }
}

static void bluetooth_panel(void)
{
    spinoedit_t *se = spinoedit();
    int status, v;
    char msg[128];
    status = bluetooth_status(msg);
    ImGui::Text("Bluetooth: %s", msg);

    switch (status) {
    case SPINO_BT_IDLE:
        if (ImGui::Button("Refresh")) bluetooth_refresh_list();
        bluetooth_get_devices(on_bt_device);
        break;
    case SPINO_BT_LOADED:
        bluetooth_get_devices(on_bt_device);
        break;
    case SPINO_BT_CONNECTED:
        v = round(se->device_angle * DR2D);
        if (ImGui::InputInt("Angle", &v, 10)) {
            while (v < 0) v += 360;
            v = v % 360;
            se->device_angle = v * DD2R;
        }
        ImGui::Checkbox("Dither", &se->dithering);
        break;
    }
}

// Adjust scroll values so that the position x,y is seen at the visible
// position vx, vy.
static void scroll_adjust(float x, float y, float vx, float vy)
{
    ImGui::SetScrollX(x - vx);
    ImGui::SetScrollY(y - vy);
}

// x, y:    pos in layer pixel.
// mx, my:  mouse pos (relative to start of panel.
// bx, by:  offset to use to draw lines.
static void image_tool_iter(int x, int y, int mx, int my, int bx, int by,
                            bool spherical,
                            int *state)
{
    enum {
        IDLE = 0,
        LINE,
        PAINT,
        PAN,
        PICK,
        WAIT_UP,
    };
    spinoedit_t *se = spinoedit();
    layer_t *layer = se->image->layer;
    bool mouse = ImGui::IsMouseDown(0);
    bool space = ImGui::IsKeyDown(' ');
    bool ctrl = ImGui::IsKeyDown(KEY_CONTROL);
    bool shift = ImGui::IsKeyDown(KEY_SHIFT);
    uint8_t color[3];
    ImVec2 c_pos;
    int i;
    int h = se->image->h;

    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    c_pos = ImGui::GetCursorScreenPos();

    // For panning and line.
    static struct {
        float mx, my;
        int x, y;
    } start;

    switch (*state) {
    case IDLE:
        spinoedit_set_hint_text(se, "ctrl: pick color. space: pan view");
        if (space) {
            start.mx = mx;
            start.my = my;
            *state = PAN;
            return;
        }
        if (ctrl) {
            *state = PICK;
            return;
        }
        if (shift) {
            *state = LINE;
            return;
        }
        if (!mouse) return;
        image_history_push(se->image);
        *state = PAINT;
        break;
    case PAINT:
        if (!mouse) {
            *state = IDLE;
            return;
        }
        for (i = 0; i < se->symmetry; i++) {
            layer_paint(
                    layer, x, y + h * i / se->symmetry,
                    se->color, se->size / 2.0,
                    se->smooth ? 0.75 : 0,
                    spherical);
        }
        spinoedit_update_layers(se);
        start.mx = mx;
        start.my = my;
        start.x = x;
        start.y = y;
        break;
    case PAN:
        // Paning the view.
        if (!space) {
            *state = IDLE;
            return;
        }
        scroll_adjust(start.mx, start.my,
                      mx - ImGui::GetScrollX(), my - ImGui::GetScrollY());
        break;
    case LINE:
        if (!shift) {
            *state = IDLE;
            return;
        }
        draw_list->AddLine(ImVec2(bx + start.mx, by + start.my),
                           ImVec2(bx + mx, by + my),
                           0xffffffff);
        if (mouse) {
            layer_paint_line(layer, start.x, start.y, x, y, se->color,
                             se->size / 2.0, se->smooth ? 0.75 : 0,
                             spherical);
            spinoedit_update_layers(se);
            start.x = x;
            start.y = y;
            start.mx = mx;
            start.my = my;
            *state = WAIT_UP;
        }
        break;
    case WAIT_UP:
        if (!mouse) *state = IDLE;
        break;
    case PICK:
        if (!ctrl) {
            *state = IDLE;
            return;
        }
        layer_get_color_at(layer, x, y, color);
        ImGui::SetTooltip("%.2f,%.2f,%3.2f",
                color[0] / 255., color[1] / 255., color[2] / 255.);
        if (mouse) memcpy(se->color, color, 3);
        break;
    }
}

static void image_panel(void)
{
    // For the moment render the image as a serie of rects??
    spinoedit_t *se = spinoedit();
    layer_t *layer = se->image->layer;
    layer_t *mlayer = se->merged_layer;
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 a, b;
    ImU32 col;
    ImVec2 c_pos;
    int i, j, x, y;
    const int s = se->zoom;

    c_pos = ImGui::GetCursorScreenPos();

    for (i = 0; i < layer->h; i++) {
        for (j = 0; j < layer->w; j++) {
            col = ImGui::GetColorU32(ImVec4(
                        mlayer->data[(i * layer->w + j) * 3 + 0] / 255.,
                        mlayer->data[(i * layer->w + j) * 3 + 1] / 255.,
                        mlayer->data[(i * layer->w + j) * 3 + 2] / 255.,
                        1));
            a = ImVec2(c_pos.x + j * s, c_pos.y + i * s);
            b = ImVec2(c_pos.x + j * s + s - 1, c_pos.y + i * s + s - 1);
            draw_list->AddRectFilled(a, b, col);
        }
    }

    ImGui::Dummy(ImVec2(layer->w * s, layer->h * s));
    if (ImGui::IsItemHovered() || se->state) {
        x = (ImGui::GetMousePos().x - c_pos.x) / s;
        y = (ImGui::GetMousePos().y - c_pos.y) / s;
        if (x >= 0 && x < layer->w && y >= 0 && y < layer->h)
            image_tool_iter(x, y, x * s, y * s,
                            c_pos.x + s / 2, c_pos.y + s / 2, false,
                            &se->state);

        if (ImGui::GetIO().MouseWheel) {
            se->zoom = clamp(se->zoom + ImGui::GetIO().MouseWheel, 4, 14);
            scroll_adjust(x * se->zoom, y * se->zoom,
                          x * s - ImGui::GetScrollX(),
                          y * s - ImGui::GetScrollY());
        }
    }
}

static void spino_panel(void)
{
    spinoedit_t *se = spinoedit();
    layer_t *layer = se->image->layer;
    layer_t *mlayer = se->merged_layer;
    ImVec2 p, c_pos;
    ImU32 col;
    int i, j, pass, x, y;
    float rtot, rpixel, r, a;
    float s = se->zoom2;
    c_pos = ImGui::GetCursorScreenPos();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 canvas_size = ImGui::GetContentRegionAvail();
    rtot = min(canvas_size.x, canvas_size.y) / 2 * s;
    rpixel = rtot / layer->w / 3;

    col = ImGui::GetColorU32(ImVec4(0.05, 0.05, 0.05, 1));
    p.x = c_pos.x + rtot;
    p.y = c_pos.y + rtot;
    draw_list->AddCircleFilled(p, rtot, col, 64);

    for (pass = 0; pass < 2; pass++)
    for (i = 0; i < layer->h; i++)
    for (j = 0; j < layer->w; j++) {
        r = rtot * j / layer->w;
        a = (float)i / layer->h * 2 * M_PI;
        a = fmod(a + se->angle, 2 * M_PI);
        p.x = c_pos.x + rtot + r * cos(a);
        p.y = c_pos.y + rtot + r * sin(a);
        col = ImGui::GetColorU32(ImVec4(
                    mlayer->data[(i * layer->w + j) * 3 + 0] / 255.,
                    mlayer->data[(i * layer->w + j) * 3 + 1] / 255.,
                    mlayer->data[(i * layer->w + j) * 3 + 2] / 255.,
                    1));
        if (pass == 0 && (col & 0x00ffffff)) continue;
        if (pass == 1 && !(col & 0x00ffffff)) continue;
        draw_list->AddCircleFilled(p, rpixel, col, 5);
    }
    ImGui::Dummy(ImVec2(rtot * 2, rtot * 2));
    if (ImGui::IsItemHovered() || se->state) {
        p.x = ImGui::GetMousePos().x - c_pos.x - rtot;
        p.y = ImGui::GetMousePos().y - c_pos.y - rtot;
        r = sqrt(p.x * p.x + p.y * p.y);
        a = atan2(p.y, p.x);
        if (a < 0) a += 2 * M_PI;
        a = fmod(a - se->angle + 2 * M_PI, 2 * M_PI);
        x = round(r / rtot * layer->w);
        y = round(a / (2 * M_PI) * layer->h);
        if (x >= 0 && x < layer->w && y >= 0 && y < layer->h)
            image_tool_iter(x, y, p.x + rtot, p.y + rtot,
                            c_pos.x, c_pos.y,
                            true, &se->state);

        if (ImGui::GetIO().MouseWheel) {
            p.x = ImGui::GetMousePos().x - c_pos.x;
            p.y = ImGui::GetMousePos().y - c_pos.y;
            se->zoom2 = clamp(se->zoom2 + ImGui::GetIO().MouseWheel / 8.,
                              1.0, 8.0);
            scroll_adjust(p.x / s * se->zoom2, p.y / s * se->zoom2,
                          p.x - ImGui::GetScrollX(),
                          p.y - ImGui::GetScrollY());
        }
    }
}

static void new_img(spinoedit_t *se, int w, int h)
{
    image_delete(se->image);
    se->image = image_create(w, h);
    image_add_layer(se->image);
}

static void load(spinoedit_t *se)
{
    char *path = NULL;
    bool result = dialog_open(DIALOG_FLAG_OPEN, "png\0*.png\0", &path);
    if (!result) return;
    spinoedit_load(se, path);
    free(path);
}

static void save_as(spinoedit_t *se)
{
    char *path = NULL;
    bool result;
    result = dialog_open(DIALOG_FLAG_SAVE, "png\0*.png\0", &path);
    if (!result) return;
    spinoedit_save(se, se->image, path);
}

static void save(spinoedit_t *se)
{
    if (!se->image->path) {
        save_as(se);
        return;
    }
    spinoedit_save(se, se->image, se->image->path);
}

static void import_image(spinoedit_t *se)
{
    char *path = NULL;
    bool result;
    result = dialog_open(DIALOG_FLAG_OPEN, NULL, &path);
    if (!result) return;
    spinoedit_import(se, path);
    free(path);
}

void gui_iter(const inputs_t *inputs)
{
    static int current_panel = 0;
    int i, left_pane_width, flags;
    ImGuiIO& io = ImGui::GetIO();
    spinoedit_t *se = spinoedit();
    io.DisplaySize = ImVec2((float)se->screen_size[0],
                            (float)se->screen_size[1]);

    // Setup time step
    io.DeltaTime = 1.0 / 60;

    io.MousePos = ImVec2(inputs->mouse_pos[0], inputs->mouse_pos[1]);
    io.MouseDown[0] = inputs->mouse_down[0];
    io.MouseDown[1] = inputs->mouse_down[1];
    io.MouseWheel = inputs->mouse_wheel;

    for (i = 0; i < ARRAY_SIZE(inputs->keys); i++)
        io.KeysDown[i] = inputs->keys[i];
    io.KeyShift = inputs->keys[KEY_SHIFT];
    io.KeyCtrl = inputs->keys[KEY_CONTROL];
    for (i = 0; i < ARRAY_SIZE(inputs->chars); i++) {
        if (!inputs->chars[i]) break;
        io.AddInputCharacter(inputs->chars[i]);
    }

    ImGui::NewFrame();

    // Create the root fullscreen window.
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar |
                                    ImGuiWindowFlags_NoResize |
                                    ImGuiWindowFlags_NoMove |
                                    ImGuiWindowFlags_MenuBar |
                                    ImGuiWindowFlags_NoCollapse;

    ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y));
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::Begin("SpinoEdit", NULL, window_flags);

    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::BeginMenu("New")) {
                if (ImGui::MenuItem("32x128")) {new_img(se, 32, 128);}
                if (ImGui::MenuItem("48x192")) {new_img(se, 48, 192);}
                if (ImGui::MenuItem("64x256")) {new_img(se, 64, 256);}
                ImGui::EndMenu();
            }
            if (ImGui::MenuItem("Save")) save(se);
            if (ImGui::MenuItem("Save as")) save_as(se);
            if (ImGui::MenuItem("Open")) load(se);
            if (ImGui::MenuItem("Import Image")) import_image(se);
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit")) {
            if (ImGui::MenuItem("Undo", "Ctrl+Z")) spinoedit_undo(se);
            if (ImGui::MenuItem("Redo", "Ctrl+Y")) spinoedit_redo(se);
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
    ImGui::Spacing();

    ImGui::BeginChild("Top",
            ImVec2(0, ImGui::GetContentRegionAvail().y - 20), true, 0);

    left_pane_width = 230;
    flags = ImGuiWindowFlags_HorizontalScrollbar |
            ImGuiWindowFlags_NoScrollWithMouse;
    ImGui::BeginChild("left pane", ImVec2(left_pane_width, 0), true, flags);
    ImGui::PushItemWidth(75);

    const struct {
        const char *name;
        void (*fn)(void);
    } PANELS[] = {
        {"Tool", tool_panel},
        {"Palette", palette_panel},
        {"layers", layers_panel},
        {"Text", text_panel},
        {"Effects", effects_panel},
        {"Emojis", emojis_panel},
        {"Connection", bluetooth_panel},
    };

    ImGui::Text("Spino Brush %s%s", SPINO_VERSION_STR,
                DEBUG ? " (debug)" : "");
    ImGui::Text("%dx%d %s",
                se->image->w, se->image->h, se->image->path ?: "");

    ImGui::BeginGroup();
    for (i = 0; i < (int)ARRAY_SIZE(PANELS); i++) {
        bool b = (current_panel == (int)i);
        if (ImGui::GoxTab(PANELS[i].name, &b))
            current_panel = i;
    }
    ImGui::EndGroup();
    ImGui::SameLine();
    ImGui::BeginGroup();

    ImGui::PushID("panel");
    PANELS[current_panel].fn();
    ImGui::PopID();

    ImGui::EndGroup();
    ImGui::EndChild();

    ImGui::SameLine();
    ImGui::BeginChild("rview", ImVec2(320, 0), false, flags);
    image_panel();
    ImGui::EndChild();

    ImGui::SameLine();
    ImGui::BeginChild("sview", ImVec2(0, 0), false, flags);
    spino_panel();
    ImGui::EndChild();

    ImGui::EndChild();
    ImGui::BeginChild("Bottom");
    ImGui::Text("%s", se->hint_text ?: "");
    ImGui::SameLine(180);
    ImGui::Text("%s", se->help_text ?: "");

    ImGui::EndChild();

    if (se->error_msg) {
        ImGui::OpenPopup("Error");
        if (ImGui::BeginPopupModal("Error", NULL,
                                   ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("%s", se->error_msg);
            if (ImGui::Button("OK")) {
                ImGui::CloseCurrentPopup();
                free(se->error_msg);
                se->error_msg = NULL;
            }
            ImGui::EndPopup();
        }
    }
    ImGui::End();

    if (    (io.KeyCtrl && ImGui::IsKeyPressed('Z', false)) ||
            ImGui::GetIO().InputCharacters[0] == 26)
        spinoedit_undo(se);
    if (    (io.KeyCtrl && ImGui::IsKeyPressed('Y', false)) ||
            ImGui::GetIO().InputCharacters[0] == 25)
        spinoedit_redo(se);
}

void gui_render(void)
{
    ImGui::Render();
}
