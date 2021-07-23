#pragma once

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

#include "imgui.h"

namespace ImGuiExt
{
    bool Spinner(const char* a_label, float a_radius, int a_thickness, const ImU32& a_color);
    bool ToggleButton(const char* a_label, bool a_state, const ImVec2& a_size = { 0.0f, 0.0f });

    void Image(const char* a_path, const ImVec2& a_size);
    bool ImageSwitchButton(const char* a_label, const char* a_pathEnabled, const char* a_pathDisabled, bool* a_state, const ImVec2& a_size);
    bool ImageToggleButton(const char* a_label, const char* a_path, bool a_state, const ImVec2& a_size);
}