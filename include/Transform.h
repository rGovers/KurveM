#pragma once

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

#include <glm/gtc/quaternion.hpp>

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

    void Serialize(tinyxml2::XMLDocument* a_doc, tinyxml2::XMLElement* a_parent) const;
    void ParseData(const tinyxml2::XMLElement* a_element);
};