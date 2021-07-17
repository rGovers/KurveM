#pragma once

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

#include "BezierCurveNode3.h"

class Object;

class SelectionControl
{
private:

protected:

public:
    SelectionControl() = delete;

    static bool PointInPoint(const glm::vec2& a_point, const glm::vec2& a_pos, float a_radius);
    static bool PointInPoint(const glm::mat4& a_viewProj, const glm::vec2& a_point, float a_radius, const glm::vec3 a_worldPoint);

    static bool NodeHandleInPoint(const glm::mat4& a_viewProj, const glm::vec2& a_point, float a_radius, const glm::mat4& a_world, const BezierCurveNode3& a_node);
    static bool NodeHandleInSelection(const glm::mat4& a_viewProj, const glm::vec2& a_start, const glm::vec2& a_end, const glm::mat4& a_world, const BezierCurveNode3& a_node);
    static bool NodeInSelection(const glm::mat4& a_viewProj, const glm::vec2& a_start, const glm::vec2& a_end, const glm::mat4& a_world, const BezierCurveNode3& a_node);
    
    static bool ObjectPointInSelection(Object* a_object, const glm::mat4& a_viewProj, const glm::vec2& a_start, const glm::vec2& a_end);
};