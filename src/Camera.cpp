#include "Camera.h"

#include <glm/ext/matrix_clip_space.hpp>

#include "Transform.h"

Camera::Camera()
{
    m_transform = new Transform();

    m_fov = 3.14159f * 0.45f;

    m_near = 0.1f;
    m_far = 100.0f;

    m_orthographic = false;
}
Camera::~Camera()
{
    delete m_transform;
}

glm::mat4 Camera::GetView() const
{
    return glm::inverse(m_transform->ToMatrix());
}
glm::mat4 Camera::GetProjection(const glm::vec2& a_size) const
{
    return GetProjection((int)a_size.x, (int)a_size.y);
}
glm::mat4 Camera::GetProjection(int a_width, int a_height) const
{
    if (m_orthographic)
    {
        const float halfWidth = a_width * 0.5f;
        const float halfHeight = a_height * 0.5f;

        const float widthScale = halfWidth / 100.0f;
        const float heightScale = halfHeight / 100.0f;

        return glm::ortho(-widthScale, widthScale, -heightScale, heightScale, -m_far, m_far);
    }
    return glm::perspective(m_fov, (float)a_width / a_height, m_near, m_far);
}

glm::vec3 Camera::GetScreenToWorld(const glm::vec3& a_screenPos, const glm::vec2& a_screenSize) const
{
    return GetScreenToWorld(a_screenPos, (int)a_screenSize.x, (int)a_screenSize.y);
}
glm::vec3 Camera::GetScreenToWorld(const glm::vec3& a_screenPos, int a_width, int a_height) const
{
    const glm::mat4 viewInv = m_transform->ToMatrix();
    const glm::mat4 projInv = glm::inverse(GetProjection(a_width, a_height));

    const glm::vec4 pos = viewInv * projInv * glm::vec4(a_screenPos, 1);

    return pos.xyz() / pos.w;
}