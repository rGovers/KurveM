#include "AxisControl.h"

e_Axis AxisControl::GetAxis(const glm::vec2& a_axis)
{
    const float upDot = glm::dot(a_axis, glm::vec2(0.0f, 1.0f));
    const float rightDot = glm::dot(a_axis, glm::vec2(1.0f, 0.0f));

    if (upDot > rightDot)
    {
        return Axis_Y;
    }

    return Axis_X;
}
e_Axis AxisControl::GetAxis(const glm::vec3& a_axis)
{
    const float forDot = glm::dot(a_axis, glm::vec3(0.0f, 0.0f, 1.0f));
    const float upDot = glm::dot(a_axis, glm::vec3(0.0f, 1.0f, 0.0f));
    const float rightDot = glm::dot(a_axis, glm::vec3(1.0f, 0.0f, 0.0f));

    if (forDot > upDot)
    {
        if (forDot > rightDot)
        {
            return Axis_Z;
        }
        else if (upDot > rightDot)
        {
            return Axis_Y;
        }
        else
        {
            return Axis_X;
        }
    }

    if (upDot > rightDot)
    {
        return Axis_Y;
    }

    return Axis_X;
}
e_Axis AxisControl::GetAxis(const glm::vec3& a_axis, const glm::mat4& a_transform)
{
    const glm::vec4 forward = a_transform * glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
    const glm::vec4 up = a_transform * glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
    const glm::vec4 right = a_transform * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

    const float forDot = glm::dot(a_axis, forward.xyz());
    const float upDot = glm::dot(a_axis, up.xyz());
    const float rightDot = glm::dot(a_axis, right.xyz());

    if (forDot > upDot)
    {
        if (forDot > rightDot)
        {
            return Axis_Z;
        }
        else if (upDot > rightDot)
        {
            return Axis_Y;
        }
        else
        {
            return Axis_X;
        }
    }

    if (upDot > rightDot)
    {
        return Axis_Y;
    }

    return Axis_X;
}

glm::vec3 AxisControl::GetAxis(e_Axis a_axis)
{
    switch (a_axis)
    {
    case Axis_X:
    {
        return glm::vec3(1.0f, 0.0f, 0.0f);
    }
    case Axis_Y:
    {
        return glm::vec3(0.0f, 1.0f, 0.0f);
    }
    case Axis_Z:
    {
        return glm::vec3(0.0f, 0.0f, 1.0f);
    }
    }

    return glm::vec3(0.0f, 0.0f, 0.0f);
}
glm::vec3 AxisControl::GetAxis(e_Axis a_axis, const glm::mat4& a_transform)
{
    switch (a_axis)
    {
    case Axis_X:
    {
        return (a_transform * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)).xyz();
    }
    case Axis_Y:
    {
        return (a_transform * glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)).xyz();
    }
    case Axis_Z:
    {
        return (a_transform * glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)).xyz();
    }
    }

    return glm::vec3(0.0f, 0.0f, 0.0f);
}