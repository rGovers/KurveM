#include "CameraController.h"

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

#include <glm/gtx/quaternion.hpp>

#include "Application.h"
#include "Camera.h"
#include "imgui.h"
#include "Transform.h"

CameraController::CameraController(Camera* a_camera)
{
    m_camera = a_camera;
}
CameraController::~CameraController()
{

}

void CameraController::Update(double a_delta)
{
    const Application* app = Application::GetInstance();
    GLFWwindow* window = app->GetWindow();

    ImGuiIO io = ImGui::GetIO();

    glm::vec3 mov = glm::vec3(0);

    Transform* camTransform = m_camera->GetTransform();

    const glm::quat camQuat = camTransform->Quaternion();
    const glm::mat4 mat = glm::toMat4(camQuat);
    const glm::vec3 camForward = mat[2].xyz();
    const glm::vec3 camUp = mat[1].xyz();
    const glm::vec3 camRight = mat[0].xyz();

    if (glfwGetKey(window, GLFW_KEY_W))
    {
        mov -= camForward;
    }
    if (glfwGetKey(window, GLFW_KEY_S))
    {
        mov += camForward;
    }
    if (glfwGetKey(window, GLFW_KEY_A))
    {
        mov -= camRight;
    }
    if (glfwGetKey(window, GLFW_KEY_D))
    {
        mov += camRight;
    }

    camTransform->Translation() += mov * 2.0f * (float)a_delta;

    const glm::vec2 mouseMove = glm::vec2(io.MouseDelta.x, io.MouseDelta.y);

    const glm::vec2 camMov = mouseMove * 0.01f;

    if (mouseMove.x != -FLT_MAX && mouseMove.y != -FLT_MAX)
    {
        camTransform->Quaternion() = glm::angleAxis(-camMov.x, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::angleAxis(camMov.y, camRight) * camQuat;
    }
}
void CameraController::FocusUpdate()
{
    const Application* app = Application::GetInstance();
    GLFWwindow* window = app->GetWindow();

    const ImGuiIO io = ImGui::GetIO();

    Transform* camTransform = m_camera->GetTransform();

    const glm::quat camQuat = camTransform->Quaternion();
    const glm::mat4 camRotMatrix = glm::toMat4(camQuat);

    const glm::vec3 camForward = camRotMatrix[2].xyz();
    const glm::vec3 camUp = camRotMatrix[1].xyz();
    const glm::vec3 camRight = camRotMatrix[0].xyz();

    camTransform->Translation() -= camForward * io.MouseWheel * 2.0f;

    if (glfwGetKey(window, GLFW_KEY_KP_1))
    {
        camTransform->Translation() = { 0.0f, 0.0f, -10.0f };
        camTransform->Quaternion() = glm::angleAxis(glm::pi<float>(), glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f))) * glm::angleAxis(glm::pi<float>() * 2.0f, glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f)));
    }
    if (glfwGetKey(window, GLFW_KEY_KP_7))
    {
        camTransform->Translation() = { 0.0f, 0.0f, 10.0f };
        camTransform->Quaternion() = glm::angleAxis(glm::pi<float>() * 2.0f, glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f))) * glm::angleAxis(glm::pi<float>() * 2.0f, glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f)));
    }
    if (glfwGetKey(window, GLFW_KEY_KP_8))
    {
        camTransform->Translation() = { 0.0f, -10.0f, 0.0f };
        camTransform->Quaternion() = glm::angleAxis(glm::pi<float>() * 2.0f, glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f))) * glm::angleAxis(glm::pi<float>() * 0.5f, glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f)));
    }
    if (glfwGetKey(window, GLFW_KEY_KP_2))
    {
        camTransform->Translation() = { 0.0f, 10.0f, 0.0f };
        camTransform->Quaternion() = glm::angleAxis(glm::pi<float>() * 2.0f, glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f))) * glm::angleAxis(glm::pi<float>() * 1.5f, glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f)));
    }
    if (glfwGetKey(window, GLFW_KEY_KP_4))
    {
        camTransform->Translation() = { 10.0f, 0.0f, 0.0f };
        camTransform->Quaternion() = glm::angleAxis(glm::pi<float>() * 0.5f, glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f))) * glm::angleAxis(glm::pi<float>() * 2.0f, glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f)));
    }
    if (glfwGetKey(window, GLFW_KEY_KP_6))
    {
        camTransform->Translation() = { -10.0f, 0.0f, 0.0f };
        camTransform->Quaternion() = glm::angleAxis(glm::pi<float>() * 1.5f, glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f))) * glm::angleAxis(glm::pi<float>() * 2.0f, glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f)));
    }

    if (glfwGetKey(window, GLFW_KEY_KP_5))
    {
        if (!m_orthoDown)
        {
            m_camera->SetOrthographic(!m_camera->IsOrthographic());
        }

        m_orthoDown = true;
    }
    else
    {
        m_orthoDown = false;
    } 
}