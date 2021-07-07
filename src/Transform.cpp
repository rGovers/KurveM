#include "Transform.h"

#include <glm/gtx/quaternion.hpp>

Transform::Transform()
{
    m_translation = glm::vec3(0.0f);
    m_rotation = glm::identity<glm::quat>();
    m_scale = glm::vec3(1.0f);
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

void Transform::Serialize(tinyxml2::XMLDocument* a_doc, tinyxml2::XMLElement* a_parent) const
{
    tinyxml2::XMLElement* transformElement = a_doc->NewElement("Transform");
    a_parent->InsertEndChild(transformElement);

    if (m_translation != glm::vec3(0))
    {
        tinyxml2::XMLElement* translationElement = a_doc->NewElement("Translation");
        transformElement->InsertEndChild(translationElement);

        tinyxml2::XMLElement* tXElement = a_doc->NewElement("X");
        translationElement->InsertEndChild(tXElement);
        tXElement->SetText(m_translation.x);
        tinyxml2::XMLElement* tYElement = a_doc->NewElement("Y");
        translationElement->InsertEndChild(tYElement);
        tYElement->SetText(m_translation.y);
        tinyxml2::XMLElement* tZElement = a_doc->NewElement("Z");
        translationElement->InsertEndChild(tZElement);
        tZElement->SetText(m_translation.z);
    }

    if (m_rotation != glm::identity<glm::quat>())
    {
        tinyxml2::XMLElement* quaternionElement = a_doc->NewElement("Quaternion");
        transformElement->InsertEndChild(quaternionElement);

        tinyxml2::XMLElement* qXElement = a_doc->NewElement("X");
        quaternionElement->InsertEndChild(qXElement);
        qXElement->SetText(m_rotation.x);
        tinyxml2::XMLElement* qYElement = a_doc->NewElement("Y");
        quaternionElement->InsertEndChild(qYElement);
        qYElement->SetText(m_rotation.y);
        tinyxml2::XMLElement* qZElement = a_doc->NewElement("Z");
        quaternionElement->InsertEndChild(qZElement);
        qZElement->SetText(m_rotation.z);
        tinyxml2::XMLElement* qWElement = a_doc->NewElement("W");
        quaternionElement->InsertEndChild(qWElement);
        qWElement->SetText(m_rotation.w);
    }
    
    if (m_scale != glm::vec3(1))
    {
        tinyxml2::XMLElement* scaleElement = a_doc->NewElement("Scale");
        transformElement->InsertEndChild(scaleElement);

        tinyxml2::XMLElement* sXElement = a_doc->NewElement("X");
        scaleElement->InsertEndChild(sXElement);
        sXElement->SetText(m_scale.x);
        tinyxml2::XMLElement* sYElement = a_doc->NewElement("Y");
        scaleElement->InsertEndChild(sYElement);
        sYElement->SetText(m_scale.y);
        tinyxml2::XMLElement* sZElement = a_doc->NewElement("Z");
        scaleElement->InsertEndChild(sZElement);
        sZElement->SetText(m_scale.z);
    }
}