#pragma once

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "tinyxml2.h"

class Transform
{
private:
    glm::vec3 m_translation;
    glm::quat m_rotation;
    glm::vec3 m_scale;

protected:

public:
    Transform();
    Transform(const Transform& a_other);
    ~Transform();

    inline glm::vec3& Translation()
    {
        return m_translation;
    }
    inline const glm::vec3& Translation() const
    {
        return m_translation;
    }
    inline glm::quat& Quaternion()
    {
        return m_rotation;
    }
    inline const glm::quat& Quaternion() const
    {
        return m_rotation;
    }
    inline glm::vec3& Scale()
    {
        return m_scale;
    }
    inline const glm::vec3& Scale() const
    {
        return m_scale;
    }

    inline glm::mat3 RotationMatrix() const
    {
        return glm::toMat3(m_rotation);
    }

    inline glm::vec3 Forward() const
    {
        return RotationMatrix()[2];
    }
    inline glm::vec3 Up() const
    {
        return RotationMatrix()[1];
    }
    inline glm::vec3 Right() const
    {
        return RotationMatrix()[0];
    }

    glm::mat4 ToMatrix() const;

    Transform& operator =(const Transform& a_other);

    void Serialize(tinyxml2::XMLDocument* a_doc, tinyxml2::XMLElement* a_parent) const;
    void ParseData(const tinyxml2::XMLElement* a_element);
};