#pragma once

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

class Transform;

class Camera
{
private:
    Transform* m_transform;

    float      m_fov;

    float      m_near;
    float      m_far;

    bool       m_orthographic;

protected:

public:
    Camera();
    ~Camera();

    inline Transform* GetTransform() const
    {
        return m_transform;
    }

    inline float GetFOV() const
    {
        return m_fov;
    }
    inline void SetFOV(float a_fov) 
    {
        m_fov = a_fov;
    }

    inline float GetNear() const
    {
        return m_near;
    }
    inline float GetFar() const
    {
        return m_far;
    }

    inline bool IsOrthographic() const
    {
        return m_orthographic;
    }
    inline void SetOrthographic(bool a_value)
    {
        m_orthographic = a_value;
    }

    glm::mat4 GetView() const;
    glm::mat4 GetProjection(int a_width, int a_height) const;
};