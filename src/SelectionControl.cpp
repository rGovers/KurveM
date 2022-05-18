#include "SelectionControl.h"

#include "BezierCurveNode2.h"
#include "BezierCurveNode3.h"
#include "Object.h"

bool SelectionControl::PointInMesh(const glm::mat4& a_transform, const Vertex* a_vertices, const unsigned int* a_indices, unsigned int a_indexCount, const glm::vec2& a_point)
{
    for (unsigned int i = 0; i < a_indexCount; i += 3)
    {
        const Vertex vertA = a_vertices[a_indices[i + 0]];
        const Vertex vertB = a_vertices[a_indices[i + 1]];
        const Vertex vertC = a_vertices[a_indices[i + 2]];

        glm::vec4 posA = a_transform * vertA.Position;
        glm::vec4 posB = a_transform * vertB.Position;
        glm::vec4 posC = a_transform * vertC.Position;

        posA /= posA.w;
        posB /= posB.w;
        posC /= posC.w;

        const float aO = glm::abs((posB.x - posA.x) * (posC.y - posA.y) - (posC.x - posA.x) * (posB.y - posB.y));

        const float aA = glm::abs((posA.x - a_point.x) * (posB.y - a_point.y) - (posB.x - a_point.x) * (posA.y - a_point.y));
        const float aB = glm::abs((posB.x - a_point.x) * (posC.y - a_point.y) - (posC.x - a_point.x) * (posB.y - a_point.y));
        const float aC = glm::abs((posC.x - a_point.x) * (posA.y - a_point.y) - (posA.x - a_point.x) * (posC.y - a_point.y));

        const float f = (aA + aB + aC) - aO;

        if (glm::abs(f) <= 0.00001f)
        {
            return true;
        }
    }

    return false;
}
bool SelectionControl::PointInMesh(const glm::mat4& a_transform, const LocalModel* a_model, const glm::vec2& a_point)
{
    const Vertex* vertices = a_model->GetVertices();
    const unsigned int* indices = a_model->GetIndices();
    const unsigned int indexCount = a_model->GetIndexCount();

    return PointInMesh(a_transform, vertices, indices, indexCount, a_point);
}

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
bool SelectionControl::NodeHandleInPoint(const glm::mat4& a_viewProj, const glm::vec2& a_point, float a_radius, const BezierCurveNode2& a_node)
{
    const glm::vec2 handlePos = a_node.GetHandlePosition();

    glm::vec4 fPos = a_viewProj * glm::vec4(handlePos.x, 0.0f, handlePos.y, 1.0f);
    fPos /= fPos.w;

    const glm::vec2 diff = a_point - fPos.xy();

    return glm::length(diff) <= a_radius;
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
    const glm::vec4 nodePos = glm::vec4(a_node.GetPosition(), 1.0f);

    glm::vec4 fPos = a_viewProj * a_world * nodePos; 
    fPos /= fPos.w;

    return fPos.x >= a_start.x && fPos.y >= a_start.y && fPos.x <= a_end.x && fPos.y <= a_end.y;
}
bool SelectionControl::NodeInSelection(const glm::mat4& a_viewProj, const glm::vec2& a_start, const glm::vec2& a_end, const glm::mat4& a_world, const BezierCurveNode2& a_node, float a_depth)
{
    const glm::vec2 pos = a_node.GetPosition();

    const glm::vec4 nodePos = glm::vec4(pos.x, a_depth, pos.y, 1.0f);

    glm::vec4 fPos = a_viewProj * a_world * nodePos; 
    fPos /= fPos.w;

    return fPos.x >= a_start.x && fPos.y >= a_start.y && fPos.x <= a_end.x && fPos.y <= a_end.y;
}

bool SelectionControl::ObjectPointInSelection(const Object* a_object, const glm::mat4& a_viewProj, const glm::vec2& a_start, const glm::vec2& a_end)
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