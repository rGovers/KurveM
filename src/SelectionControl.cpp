#include "SelectionControl.h"

#include "Object.h"

bool SelectionControl::PointInPoint(const glm::vec2& a_point, const glm::vec2& a_pos, float a_radius)
{
    const glm::vec2 diff = a_pos - a_point;

    return glm::length(diff) <= a_radius;
}
bool SelectionControl::PointInPoint(const glm::mat4& a_viewProj, const glm::vec2& a_point, float a_radius, const glm::vec3 a_worldPoint)
{
    const glm::vec4 pos = glm::vec4(a_worldPoint, 1);

    glm::vec4 fPos = a_viewProj * pos;
    fPos /= fPos.w;

    const glm::vec2 diff = fPos.xy() - a_point;
    const float len = glm::length(diff);

    if (len <= a_radius)
    {
        return true;
    }

    return false;
}

bool SelectionControl::NodeHandleInPoint(const glm::mat4& a_viewProj, const glm::vec2& a_point, float a_radius, const glm::mat4& a_world, const BezierCurveNode3& a_node)
{
    const glm::vec4 handlePos = glm::vec4(a_node.GetHandlePosition(), 1);

    glm::vec4 fPos = a_viewProj * a_world * handlePos; 
    fPos /= fPos.w;

    const glm::vec2 diff = fPos.xy() - a_point;
    const float len = glm::length(diff);

    if (len <= a_radius)
    {
        return true;
    }

    return false;
}
bool SelectionControl::NodeHandleInSelection(const glm::mat4& a_viewProj, const glm::vec2& a_start, const glm::vec2& a_end, const glm::mat4& a_world, const BezierCurveNode3& a_node)
{
    const glm::vec4 handlePos = glm::vec4(a_node.GetHandlePosition(), 1);

    glm::vec4 fPos = a_viewProj * a_world * handlePos; 
    fPos /= fPos.w;

    if (fPos.x >= a_start.x && fPos.y >= a_start.y &&
    fPos.x <= a_end.x && fPos.y <= a_end.y)
    {
        return true;
    }

    return false;
}
bool SelectionControl::NodeInSelection(const glm::mat4& a_viewProj, const glm::vec2& a_start, const glm::vec2& a_end, const glm::mat4& a_world, const BezierCurveNode3& a_node)
{
    const glm::vec4 nodePos = glm::vec4(a_node.GetPosition(), 1);

    glm::vec4 fPos = a_viewProj * a_world * nodePos; 
    fPos /= fPos.w;

    if (fPos.x >= a_start.x && fPos.y >= a_start.y &&
    fPos.x <= a_end.x && fPos.y <= a_end.y)
    {
        return true;
    }

    return false;
}

bool SelectionControl::ObjectPointInSelection(Object* a_object, const glm::mat4& a_viewProj, const glm::vec2& a_start, const glm::vec2& a_end)
{
    const glm::mat4 mat = a_object->GetGlobalMatrix();

    const glm::vec4 pos = mat[3];

    glm::vec4 fPos = a_viewProj * pos;
    fPos /= fPos.w;

    if (fPos.x >= a_start.x && fPos.y >= a_start.y &&
    fPos.x <= a_end.x && fPos.y <= a_end.y)
    {
        return true;
    }

    return false;
}