#pragma once

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

#include <glm/gtc/quaternion.hpp>

class Transform
{
private:
    glm::vec3 m_translation;
    glm::quat m_rotation;
    glm::vec3 m_scale;

protected:

public:
    Transform();
    ~Transform();

    inline glm::vec3& Translation()
    {
        return m_translation;
    }
    inline glm::quat& Quaternion()
    {
        return m_rotation;
    }
    inline glm::vec3& Scale()
    {
        return m_scale;
    }

    glm::mat4 ToMatrix() const;
};