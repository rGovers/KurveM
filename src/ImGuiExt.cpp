#include "ImGuiExt.h"

#include "Datastore.h"
#include "imgui_internal.h"
#include "Texture.h"

bool ImGuiExt::Spinner(const char* a_label, float a_radius, int a_thickness, const glm::vec4& a_color)
{
    Spinner(a_label, a_radius, a_thickness, ImGui::ColorConvertFloat4ToU32(ImVec4(a_color.x, a_color.y, a_color.z, a_color.w)));
}
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
    int start = glm::abs(ImSin(g.Time*1.8f)*(num_segments-5));
    
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

bool ImGuiExt::ColoredButton(const char* a_label, const glm::vec4& a_color, const glm::vec2& a_size)
{
    ColoredButton(a_label, ImVec4(a_color.x, a_color.y, a_color.z, a_color.w), ImVec2(a_size.x, a_size.y));
}
bool ImGuiExt::ColoredButton(const char* a_label, const ImVec4& a_color, const ImVec2& a_size)
{
    ImGuiStyle& style = ImGui::GetStyle();

    const ImVec4 color = style.Colors[ImGuiCol_Button];

    style.Colors[ImGuiCol_Button] = a_color;

    const bool ret = ImGui::Button(a_label, a_size);

    style.Colors[ImGuiCol_Button] = color;

    return ret;
}

bool ImGuiExt::ToggleButton(const char* a_label, bool a_state, const glm::vec2& a_size)
{
    ToggleButton(a_label, a_state, ImVec2(a_size.x, a_size.y));
}
bool ImGuiExt::ToggleButton(const char* a_label, bool a_state, const ImVec2& a_size)
{
    ImGuiStyle& style = ImGui::GetStyle();

    const ImVec4 aColor = style.Colors[ImGuiCol_ButtonActive];
    const ImVec4 color = style.Colors[ImGuiCol_Button];

    // Ghetto but it gives me toggle buttons
    if (a_state)
    {
        style.Colors[ImGuiCol_Button] = aColor;
    }

    const bool ret = ImGui::Button(a_label, a_size);

    style.Colors[ImGuiCol_Button] = color;

    return ret;
}

bool ImGuiExt::BeginImageCombo(const char* a_label, const char* a_previewPath, const glm::vec2& a_previewSize, const char* a_previewValue, ImGuiComboFlags a_flags)
{
    BeginImageCombo(a_label, a_previewPath, ImVec2(a_previewSize.x, a_previewSize.y), a_previewValue, a_flags);
}
// Ripped and apapted from ImGui BeginCombo
bool ImGuiExt::BeginImageCombo(const char* a_label, const char* a_previewPath, const ImVec2& a_previewSize, const char* a_previewValue, ImGuiComboFlags a_flags)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = ImGui::GetCurrentWindow();

    ImGuiNextWindowDataFlags backup_next_window_data_flags = g.NextWindowData.Flags;
    g.NextWindowData.ClearFlags(); // We behave like Begin() and need to consume those values
    if (window->SkipItems)
    {
        return false;
    }

    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(a_label);
    IM_ASSERT((a_flags & (ImGuiComboFlags_NoArrowButton | ImGuiComboFlags_NoPreview)) != (ImGuiComboFlags_NoArrowButton | ImGuiComboFlags_NoPreview)); // Can't use both flags together

    const float arrow_size = (a_flags & ImGuiComboFlags_NoArrowButton) ? 0.0f : ImGui::GetFrameHeight();
    const ImVec2 label_size = ImGui::CalcTextSize(a_label, NULL, true);
    const float w = (a_flags & ImGuiComboFlags_NoPreview) ? arrow_size : ImGui::CalcItemWidth();
    const ImVec2 bRM = ImVec2(window->DC.CursorPos.x + w, window->DC.CursorPos.y + label_size.y + style.FramePadding.y * 2.0f);
    const ImRect bb(window->DC.CursorPos, bRM);
    const ImVec2 tBRM = ImVec2(bb.Max.x + label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, bb.Max.y);
    const ImRect total_bb(bb.Min, tBRM);
    ImGui::ItemSize(total_bb, style.FramePadding.y);
    // if (!ImGui::ItemAdd(total_bb, id, &bb))
    // {
    //     return false;
    // }

    // Open on click
    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held);
    const ImGuiID popup_id = ImHashStr("##ComboPopup", 0, id);
    bool popup_open = ImGui::IsPopupOpen(popup_id, ImGuiPopupFlags_None);
    if ((pressed || g.NavActivateId == id) && !popup_open)
    {
        ImGui::OpenPopupEx(popup_id, ImGuiPopupFlags_None);
        popup_open = true;
    }

    // Render shape
    const ImU32 frame_col = ImGui::GetColorU32(hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
    const float value_x2 = ImMax(bb.Min.x, bb.Max.x - arrow_size);
    ImGui::RenderNavHighlight(bb, id);
    if (!(a_flags & ImGuiComboFlags_NoPreview))
        window->DrawList->AddRectFilled(bb.Min, ImVec2(value_x2, bb.Max.y), frame_col, style.FrameRounding, (a_flags & ImGuiComboFlags_NoArrowButton) ? ImDrawFlags_RoundCornersAll : ImDrawFlags_RoundCornersLeft);
    if (!(a_flags & ImGuiComboFlags_NoArrowButton))
    {
        ImU32 bg_col = ImGui::GetColorU32((popup_open || hovered) ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
        ImU32 text_col = ImGui::GetColorU32(ImGuiCol_Text);
        window->DrawList->AddRectFilled(ImVec2(value_x2, bb.Min.y), bb.Max, bg_col, style.FrameRounding, (w <= arrow_size) ? ImDrawFlags_RoundCornersAll : ImDrawFlags_RoundCornersRight);
        if (value_x2 + arrow_size - style.FramePadding.x <= bb.Max.x)
        {
            ImGui::RenderArrow(window->DrawList, ImVec2(value_x2 + style.FramePadding.y, bb.Min.y + style.FramePadding.y), text_col, ImGuiDir_Down, 1.0f);
        }
    }
    ImGui::RenderFrameBorder(bb.Min, bb.Max, style.FrameRounding);

    // Render preview and label
    if (a_previewValue != NULL && !(a_flags & ImGuiComboFlags_NoPreview))
    {
        if (g.LogEnabled)
        {
            ImGui::LogSetNextTextDecoration("{", "}");
        }

        ImVec2 preview_text_min;
        
        Texture* tex = Datastore::GetTexture(a_previewPath);
        if (tex != nullptr)
        {
            const ImVec2 imageMin = ImVec2(bb.Min.x + style.FramePadding.x, bb.Min.y + style.FramePadding.y);

            window->DrawList->AddImage((ImTextureID)tex->GetHandle(), ImVec2(imageMin.x + 1, imageMin.y + 1), ImVec2(imageMin.x + a_previewSize.x - 2, imageMin.y + a_previewSize.y - 2));
            preview_text_min = ImVec2(bb.Min.x + a_previewSize.x + style.FramePadding.x * 2, bb.Min.y + style.FramePadding.y);
        }
        else
        {
            preview_text_min = ImVec2(bb.Min.x + style.FramePadding.x, bb.Min.y + style.FramePadding.y);
        }

        ImGui::RenderTextClipped(preview_text_min, ImVec2(value_x2, bb.Max.y), a_previewValue, NULL, NULL);    
    }
    if (label_size.x > 0)
        ImGui::RenderText(ImVec2(bb.Max.x + style.ItemInnerSpacing.x, bb.Min.y + style.FramePadding.y), a_label);

    if (!popup_open)
        return false;

    g.NextWindowData.Flags = backup_next_window_data_flags;
    return ImGui::BeginComboPopup(popup_id, bb, a_flags);
}

bool ImGuiExt::Image(const char* a_path, const glm::vec2& a_size)
{
    Image(a_path, ImVec2(a_size.x, a_size.y));
}
bool ImGuiExt::Image(const char* a_path, const ImVec2& a_size)
{
    Texture* tex = Datastore::GetTexture(a_path);
    if (tex != nullptr)
    {
        ImGui::Image((ImTextureID)tex->GetHandle(), a_size);

        return true;
    }

    return false;
}

bool ImGuiExt::ImageButton(const char* a_label, const char* a_path, const glm::vec2& a_size)
{
    ImageButton(a_label, a_path, ImVec2(a_size.x, a_size.y));
}
bool ImGuiExt::ImageButton(const char* a_label, const char* a_path, const ImVec2& a_size)
{
    Texture* tex = Datastore::GetTexture(a_path);

    if (tex != nullptr)
    {
        const ImGuiID id = ImGui::GetID(a_label);

        ImGui::PushID(id);
        const bool ret = ImGui::ImageButton((ImTextureID)tex->GetHandle(), a_size);
        ImGui::PopID();

        return ret;
    }
    else
    {
        return ImGui::Button(a_label, a_size);
    }
}

bool ImGuiExt::ImageSwitchButton(const char* a_label, const char* a_pathEnabled, const char* a_pathDisabled, bool* a_state, const glm::vec2& a_size, bool a_background)
{
    ImageSwitchButton(a_label, a_pathEnabled, a_pathDisabled, a_state, ImVec2(a_size.x, a_size.y), a_background);
}
bool ImGuiExt::ImageSwitchButton(const char* a_label, const char* a_pathEnabled, const char* a_pathDisabled, bool* a_state, const ImVec2& a_size, bool a_background)
{
    ImGuiStyle& style = ImGui::GetStyle();

    const ImVec4 color = style.Colors[ImGuiCol_Button];
    const ImVec4 hColor = style.Colors[ImGuiCol_ButtonHovered];
    const ImVec4 aColor = style.Colors[ImGuiCol_ButtonActive];

    if (!a_background)
    {
        style.Colors[ImGuiCol_Button] = ImVec4(0, 0, 0, 0);
        style.Colors[ImGuiCol_ButtonActive] = ImVec4(0, 0, 0, 0);
    }

    const char* path = a_pathDisabled;

    if (*a_state)
    {
        path = a_pathEnabled;
    }

    bool ret = false;

    Texture* tex = Datastore::GetTexture(path);
    if (tex != nullptr)
    {
        const ImGuiID id = ImGui::GetID(a_label);

        ImGui::PushID(id);
        ret = ImGui::ImageButton((ImTextureID)tex->GetHandle(), a_size);
        ImGui::PopID();

        if (ret)
        {
            *a_state = !*a_state;
        }
    }
    else
    {
        ImGui::Checkbox(a_label, a_state);
    }

    style.Colors[ImGuiCol_Button] = color;
    style.Colors[ImGuiCol_ButtonActive] = aColor;

    return ret;
}

bool ImGuiExt::ImageToggleButton(const char* a_label, const char* a_path, bool a_state, const glm::vec2& a_size)
{
    ImageToggleButton(a_label, a_path, a_state, ImVec2(a_size.x, a_size.y));
}
bool ImGuiExt::ImageToggleButton(const char* a_label, const char* a_path, bool a_state, const ImVec2& a_size)
{
    ImGuiStyle& style = ImGui::GetStyle();

    const ImVec4 aColor = style.Colors[ImGuiCol_ButtonActive];
    const ImVec4 color = style.Colors[ImGuiCol_Button];

    // Ghetto but it gives me toggle buttons
    if (a_state)
    {
        ImGui::GetStyle().Colors[ImGuiCol_Button] = aColor;
    }

    bool ret = false;
    Texture* tex = Datastore::GetTexture(a_path);
    if (tex != nullptr)
    {
        const ImGuiID id = ImGui::GetID(a_label);

        ImGui::PushID(id);
        ret = ImGui::ImageButton((ImTextureID)tex->GetHandle(), a_size);
        ImGui::PopID();
    }
    else
    {
        ret = ImGui::Button(a_label, a_size);
    }

    ImGui::GetStyle().Colors[ImGuiCol_Button] = color;

    return ret;
}