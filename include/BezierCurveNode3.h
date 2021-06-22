#pragma once

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

class BezierCurveNode3
{
private:
    glm::vec3 m_pos;
    glm::vec3 m_handle;

protected:

public:
    BezierCurveNode3();
    BezierCurveNode3(const glm::vec3& a_pos, const glm::vec3& a_handlePos);
    ~BezierCurveNode3();

    glm::vec3& Position() 
    {
        return m_pos;
    }
    glm::vec3& HandlePosition()
    {
        return m_handle;
    }

    glm::vec3 GetPoint(const BezierCurveNode3& a_other, float a_lerp) const;
    static glm::vec3 GetPoint(const BezierCurveNode3& a_pointA, const BezierCurveNode3& a_pointB, float a_lerp);
};