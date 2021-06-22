#include "SelectionControl.h"

bool SelectionControl::NodeHandleInSelection(const glm::mat4& a_viewProj, const glm::vec2& a_start, const glm::vec2& a_end, const glm::mat4& a_world, BezierCurveNode3 a_node)
{
    const glm::vec4 handlePos = glm::vec4(a_node.HandlePosition(), 1);

    glm::vec4 fPos = a_viewProj * a_world * handlePos; 
    fPos /= fPos.w;

    if (fPos.x >= a_start.x && fPos.y >= a_start.y &&
    fPos.x <= a_end.x && fPos.y <= a_end.y)
    {
        return true;
    }

    return false;
}
bool SelectionControl::NodeInSelection(const glm::mat4& a_viewProj, const glm::vec2& a_start, const glm::vec2& a_end, const glm::mat4& a_world, BezierCurveNode3 a_node)
{
    const glm::vec4 nodePos = glm::vec4(a_node.Position(), 1);

    glm::vec4 fPos = a_viewProj * a_world * nodePos; 
    fPos /= fPos.w;

    if (fPos.x >= a_start.x && fPos.y >= a_start.y &&
    fPos.x <= a_end.x && fPos.y <= a_end.y)
    {
        return true;
    }

    return false;
}