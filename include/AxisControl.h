#pragma once

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

enum e_Axis
{
    Axis_X,
    Axis_Y,
    Axis_Z
};

class AxisControl
{
private:

protected:

public:
    AxisControl() = delete;

    static e_Axis GetAxis(const glm::vec2& a_axis);
    static e_Axis GetAxis(const glm::vec3& a_axis);
    static e_Axis GetAxis(const glm::vec3& a_axis, const glm::mat4& a_transform);

    static glm::vec3 GetAxis(e_Axis a_axis);
    static glm::vec3 GetAxis(e_Axis a_axis, const glm::mat4& a_transform);
};