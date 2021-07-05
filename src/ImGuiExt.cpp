#include "ImGuiExt.h"

#include "imgui_internal.h"

// https://github.com/ocornut/imgui/issues/1901
bool ImGuiExt::Spinner(const char* a_label, float a_radius, int a_thickness, const ImU32& a_color) 
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();

    if (window->SkipItems)
    {
        return false;
    }
    
    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(a_label);
    
    ImVec2 pos = window->DC.CursorPos;
    ImVec2 size((a_radius) * 2, (a_radius + style.FramePadding.y) * 2);
    
    const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
    ImGui::ItemSize(bb, style.FramePadding.y);
    if (!ImGui::ItemAdd(bb, id))
    {
        return false;
    }
    
    // Render
    window->DrawList->PathClear();
    
    int num_segments = 30;
    int start = abs(ImSin(g.Time*1.8f)*(num_segments-5));
    
    const float a_min = IM_PI * 2.0f * ((float)start) / (float)num_segments;
    const float a_max = IM_PI * 2.0f * ((float)num_segments - 3) / (float)num_segments;
    const ImVec2 centre = ImVec2(pos.x + a_radius, pos.y + a_radius + style.FramePadding.y);
    
    for (int i = 0; i < num_segments; i++) 
    {
        const float a = a_min + ((float)i / (float)num_segments) * (a_max - a_min);
        window->DrawList->PathLineTo(ImVec2(centre.x + ImCos(a+g.Time * 8) * a_radius,
                                            centre.y + ImSin(a+g.Time * 8) * a_radius));
    }

    window->DrawList->PathStroke(a_color, false, a_thickness);
}

bool ImGuiExt::ToggleButton(const char* a_label, bool a_state, const ImVec2& a_size)
{
    ImGuiStyle& style = ImGui::GetStyle();

    const ImVec4 aColor = style.Colors[ImGuiCol_ButtonActive];
    const ImVec4 color = style.Colors[ImGuiCol_Button];

    // Ghetto but it gives me toggle buttons
    if (a_state)
    {
        ImGui::GetStyle().Colors[ImGuiCol_Button] = aColor;
    }

    const bool ret = ImGui::Button(a_label, a_size);

    ImGui::GetStyle().Colors[ImGuiCol_Button] = color;

    return ret;
}
