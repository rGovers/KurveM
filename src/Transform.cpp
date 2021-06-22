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