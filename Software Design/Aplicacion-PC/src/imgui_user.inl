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

extern "C" {
#include "spino.h"
}

namespace ImGui {
    bool GoxTab(const char *text, bool *v)
    {
        ImFont *font = GImGui->Font;
        const ImFont::Glyph *glyph;
        char c;
        bool ret;
        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        float pad = style.FramePadding.x;
        ImVec4 color;
        ImVec2 text_size = CalcTextSize(text);
        ImGuiWindow* window = GetCurrentWindow();
        ImVec2 pos = window->DC.CursorPos + ImVec2(pad, text_size.x + pad);

        color = style.Colors[ImGuiCol_Button];
        if (*v) color = style.Colors[ImGuiCol_ButtonActive];
        ImGui::PushStyleColor(ImGuiCol_Button, color);
        ImGui::PushID(text);
        ret = ImGui::Button("", ImVec2(text_size.y + pad * 2,
                                       text_size.x + pad * 2));
        ImGui::PopStyleColor();
        while ((c = *text++)) {
            glyph = font->FindGlyph(c);
            if (!glyph) continue;

            window->DrawList->PrimReserve(6, 4);
            window->DrawList->PrimQuadUV(
                    pos + ImVec2(glyph->Y0, -glyph->X0),
                    pos + ImVec2(glyph->Y0, -glyph->X1),
                    pos + ImVec2(glyph->Y1, -glyph->X1),
                    pos + ImVec2(glyph->Y1, -glyph->X0),

                    ImVec2(glyph->U0, glyph->V0),
                    ImVec2(glyph->U1, glyph->V0),
                    ImVec2(glyph->U1, glyph->V1),
                    ImVec2(glyph->U0, glyph->V1),
                    0xFFFFFFFF);
            pos.y -= glyph->XAdvance;
        }
        ImGui::PopID();
        return ret;
    }
};
