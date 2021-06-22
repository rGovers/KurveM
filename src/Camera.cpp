#include "Camera.h"

#include <glm/ext/matrix_clip_space.hpp>

#include "Transform.h"

Camera::Camera()
{
    m_transform = new Transform();

    m_fov = 3.14159f * 0.45f;

    m_near = 0.1f;
    m_far = 100.0f;
}
Camera::~Camera()
{
    delete m_transform;
}

glm::mat4 Camera::GetView() const
{
    return glm::inverse(m_transform->ToMatrix());
}
glm::mat4 Camera::GetProjection(int a_width, int a_height) const
{
    return glm::perspective(m_fov, (float)a_width / a_height, m_near, m_far);
}