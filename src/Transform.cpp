#include "Transform.h"

#include <glm/gtx/quaternion.hpp>

#include "XMLIO.h"

Transform::Transform()
{
    m_translation = glm::vec3(0.0f);
    m_rotation = glm::identity<glm::quat>();
    m_scale = glm::vec3(1.0f);
}
Transform::Transform(const Transform& a_other)
{
    m_translation = a_other.m_translation;
    m_rotation = a_other.m_rotation;
    m_scale = a_other.m_scale;
}
Transform::~Transform()
{

}

glm::mat4 Transform::ToMatrix() const
{
    const glm::mat4 iden = glm::identity<glm::mat4>();

    const glm::mat4 translation = glm::translate(iden, m_translation);
    const glm::mat4 rotation = glm::toMat4(m_rotation);
    const glm::mat4 scale = glm::scale(iden, m_scale);

    return translation * rotation * scale;
}

Transform& Transform::operator =(const Transform& a_other)
{
    m_translation = a_other.m_translation;
    m_rotation = a_other.m_rotation;
    m_scale = a_other.m_scale;

    return *this;
}

void Transform::Serialize(tinyxml2::XMLDocument* a_doc, tinyxml2::XMLElement* a_parent) const
{
    tinyxml2::XMLElement* transformElement = a_doc->NewElement("Transform");
    a_parent->InsertEndChild(transformElement);

    if (m_translation != glm::vec3(0))
    {
        XMLIO::WriteVec3(a_doc, transformElement, "Translation", m_translation);
    }
    if (m_rotation != glm::identity<glm::quat>())
    {
        XMLIO::WriteQuat(a_doc, transformElement, "Quaternion", m_rotation);
    }
    if (m_scale != glm::vec3(1))
    {
        XMLIO::WriteVec3(a_doc, transformElement, "Scale", m_scale, glm::vec3(1));
    }
}
void Transform::ParseData(const tinyxml2::XMLElement* a_element)
{
    for (const tinyxml2::XMLElement* iter = a_element->FirstChildElement(); iter != nullptr; iter = iter->NextSiblingElement())
    {
        const char* str = iter->Value();

        if (strcmp(str, "Translation") == 0)
        {
            XMLIO::ReadVec3(iter, &m_translation);
        }
        else if (strcmp(str, "Quaternion") == 0)
        {
            XMLIO::ReadQuat(iter, &m_rotation);
        }
        else if (strcmp(str, "Scale") == 0)
        {
            XMLIO::ReadVec3(iter, &m_scale);
        }
    }
}