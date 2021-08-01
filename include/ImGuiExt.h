#pragma once

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

#include "imgui.h"

namespace ImGuiExt
{
    bool Spinner(const char* a_label, float a_radius, int a_thickness, const glm::vec4& a_color);
    bool Spinner(const char* a_label, float a_radius, int a_thickness, const ImU32& a_color);

    bool ColoredButton(const char* a_label, const glm::vec4& a_color, const glm::vec2& a_size = { 0, 0 });
    bool ColoredButton(const char* a_label, const ImVec4& a_color, const ImVec2& a_size = { 0, 0 });

    bool ToggleButton(const char* a_label, bool a_state, const glm::vec2& a_size);
    bool ToggleButton(const char* a_label, bool a_state, const ImVec2& a_size = { 0.0f, 0.0f });

    bool BeginImageCombo(const char* a_label, const char* a_previewPath, const glm::vec2& a_previewSize, const char* a_previewValue, ImGuiComboFlags a_flags = 0);
    bool BeginImageCombo(const char* a_label, const char* a_previewPath, const ImVec2& a_previewSize, const char* a_previewValue, ImGuiComboFlags a_flags = 0);

    bool Image(const char* a_path, const glm::vec2& a_size);
    bool Image(const char* a_path, const ImVec2& a_size);

    bool ImageButton(const char* a_label, const char* a_path, const glm::vec2& a_size);
    bool ImageButton(const char* a_label, const char* a_path, const ImVec2& a_size);

    bool ImageSwitchButton(const char* a_label, const char* a_pathEnabled, const char* a_pathDisabled, bool* a_state, const glm::vec2& a_size, bool a_background = false);
    bool ImageSwitchButton(const char* a_label, const char* a_pathEnabled, const char* a_pathDisabled, bool* a_state, const ImVec2& a_size, bool a_background = false);

    bool ImageToggleButton(const char* a_label, const char* a_path, bool a_state, const glm::vec2& a_size);
    bool ImageToggleButton(const char* a_label, const char* a_path, bool a_state, const ImVec2& a_size);
}