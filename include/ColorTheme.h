#pragma once

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

struct ColorTheme
{
    static glm::vec4 Active;
    static glm::vec4 InActive;
    static glm::vec4 Hovered;
};