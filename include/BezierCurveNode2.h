#pragma once

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

class BezierCurveNode2
{
private:
    glm::vec2 m_pos;
    glm::vec2 m_handle;

protected:

public:
    BezierCurveNode2();
    BezierCurveNode2(const glm::vec2& a_pos, const glm::vec2& a_handlePos);
    ~BezierCurveNode2();

    inline glm::vec2 GetPosition() const
    {
        return m_pos;
    }
    inline void SetPosition(const glm::vec2& a_position)
    {
        m_pos = a_position;
    }

    inline glm::vec2 GetHandlePosition() const
    {
        return m_handle;
    }
    inline void SetHandlePosition(const glm::vec2& a_handle)
    {
        m_handle = a_handle;
    }

    glm::vec2 GetPoint(const BezierCurveNode2& a_other, float a_lerp) const;
    glm::vec2 GetPointScaled(const BezierCurveNode2& a_other, float a_scale, float a_lerp) const;

    static glm::vec2 GetPoint(const BezierCurveNode2& a_pointA, const BezierCurveNode2& a_pointB, float a_lerp);
    static glm::vec2 GetPointScaled(const BezierCurveNode2& a_pointA, const BezierCurveNode2& a_pointB, float a_scale, float a_lerp);
};