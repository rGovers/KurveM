#pragma once

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

#include "LocalModel.h"

struct BezierCurveNode2;
struct BezierCurveNode3;

class Object;

class SelectionControl
{
private:

protected:

public:
    SelectionControl() = delete;

    static bool PointInMesh(const glm::mat4& a_transform, const Vertex* a_vertices, const unsigned int* a_indices, unsigned int a_indexCount, const glm::vec2& a_point);
    static bool PointInMesh(const glm::mat4& a_transform, const LocalModel* a_model, const glm::vec2& a_point);

    static bool PointInPoint(const glm::vec2& a_point, const glm::vec2& a_pos, float a_radius);
    static bool PointInPoint(const glm::mat4& a_viewProj, const glm::vec2& a_point, float a_radius, const glm::vec3 a_worldPoint);

    static bool NodeHandleInPoint(const glm::mat4& a_viewProj, const glm::vec2& a_point, float a_radius, const BezierCurveNode2& a_node);
    static bool NodeHandleInPoint(const glm::mat4& a_viewProj, const glm::vec2& a_point, float a_radius, const glm::mat4& a_world, const BezierCurveNode3& a_node);
    static bool NodeHandleInSelection(const glm::mat4& a_viewProj, const glm::vec2& a_start, const glm::vec2& a_end, const glm::mat4& a_world, const BezierCurveNode3& a_node);

    static bool NodeInSelection(const glm::mat4& a_viewProj, const glm::vec2& a_start, const glm::vec2& a_end, const glm::mat4& a_world, const BezierCurveNode3& a_node);
    static bool NodeInSelection(const glm::mat4& a_viewProj, const glm::vec2& a_start, const glm::vec2& a_end, const glm::mat4& a_world, const BezierCurveNode2& a_node, float a_depth = 0.0f);
    
    static bool ObjectPointInSelection(const Object* a_object, const glm::mat4& a_viewProj, const glm::vec2& a_start, const glm::vec2& a_end);
};