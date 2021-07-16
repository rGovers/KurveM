#pragma once

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

class BezierCurveNode3
{
private:
    glm::vec3 m_pos;
    glm::vec3 m_handle;
    glm::vec2 m_uv;

protected:

public:
    BezierCurveNode3();
    BezierCurveNode3(const glm::vec3& a_pos, const glm::vec3& a_handlePos, const glm::vec2& a_uv = glm::vec2(0));
    ~BezierCurveNode3();

    inline glm::vec3 GetPosition() const
    {
        return m_pos;
    }
    inline void SetPosition(const glm::vec3& a_position)
    {
        m_pos = a_position;
    }

    inline glm::vec3 GetHandlePosition() const
    {
        return m_handle;
    }
    inline void SetHandlePosition(const glm::vec3& a_handle)
    {
        m_handle = a_handle;
    }

    inline glm::vec2 GetUV() const
    {
        return m_uv;
    }
    inline void SetUV(const glm::vec2& a_uv)
    {
        m_uv = a_uv;
    }

    glm::vec2 GetUVLerp(const BezierCurveNode3& a_other, float a_lerp) const;

    glm::vec3 GetPoint(const BezierCurveNode3& a_other, float a_lerp) const;
    glm::vec3 GetPointScaled(const BezierCurveNode3& a_other, float a_scale, float a_lero) const;

    static glm::vec2 GetUVLerp(const BezierCurveNode3& a_pointA, const BezierCurveNode3& a_pointB, float a_lerp);

    static glm::vec3 GetPoint(const BezierCurveNode3& a_pointA, const BezierCurveNode3& a_pointB, float a_lerp);
    static glm::vec3 GetPointScaled(const BezierCurveNode3& a_pointA, const BezierCurveNode3& a_pointB, float a_scale, float a_lerp);
};