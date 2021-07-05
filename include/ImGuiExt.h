#pragma once

#include "imgui.h"

namespace ImGuiExt
{
    bool Spinner(const char* a_label, float a_radius, int a_thickness, const ImU32& a_color);
    bool ToggleButton(const char* a_label, bool a_state, const ImVec2& a_size = { 0.0f, 0.0f });
}