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
    constexpr glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    const Application* app = Application::GetInstance();
    GLFWwindow* window = app->GetWindow();

    const ImGuiIO io = ImGui::GetIO();

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
    if (glfwGetKey(window, GLFW_KEY_SPACE))
    {
        mov -= camUp;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT))
    {
        mov += camUp;
    }

    camTransform->Translation() += mov * 2.0f * (float)a_delta;

    const glm::vec2 mouseMove = glm::vec2(io.MouseDelta.x, io.MouseDelta.y);

    const glm::vec2 camMov = mouseMove * 0.01f;

    if (mouseMove.x != -FLT_MAX && mouseMove.y != -FLT_MAX)
    {
        camTransform->Quaternion() = glm::angleAxis(-camMov.x, up) * glm::angleAxis(camMov.y, camRight) * camQuat;
    }
}
void CameraController::FocusUpdate()
{
    constexpr float pi = glm::pi<float>();
    constexpr float piHalf = pi * 0.5f;
    constexpr float pi2 = pi * 2;

    constexpr glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f);
    constexpr glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    const Application* app = Application::GetInstance();
    GLFWwindow* window = app->GetWindow();

    const ImGuiIO io = ImGui::GetIO();

    Transform* camTransform = m_camera->GetTransform();

    const glm::quat camQuat = camTransform->Quaternion();
    const glm::mat4 camRotMatrix = glm::toMat4(camQuat);

    const glm::vec3 camForward = camRotMatrix[2].xyz();

    camTransform->Translation() -= camForward * io.MouseWheel * 2.0f;

    if (glfwGetKey(window, GLFW_KEY_KP_1))
    {
        camTransform->Translation() = { 0.0f, 0.0f, -10.0f };
        camTransform->Quaternion() = glm::angleAxis(pi, up) * glm::angleAxis(pi2, right);
    }
    if (glfwGetKey(window, GLFW_KEY_KP_7))
    {
        camTransform->Translation() = { 0.0f, 0.0f, 10.0f };
        camTransform->Quaternion() = glm::angleAxis(pi2, up) * glm::angleAxis(pi2, right);
    }
    if (glfwGetKey(window, GLFW_KEY_KP_8))
    {
        camTransform->Translation() = { 0.0f, -10.0f, 0.0f };
        camTransform->Quaternion() = glm::angleAxis(pi2, up) * glm::angleAxis(piHalf, right);
    }
    if (glfwGetKey(window, GLFW_KEY_KP_2))
    {
        camTransform->Translation() = { 0.0f, 10.0f, 0.0f };
        camTransform->Quaternion() = glm::angleAxis(pi2, up) * glm::angleAxis(pi + piHalf, right);
    }
    if (glfwGetKey(window, GLFW_KEY_KP_4))
    {
        camTransform->Translation() = { 10.0f, 0.0f, 0.0f };
        camTransform->Quaternion() = glm::angleAxis(piHalf, up) * glm::angleAxis(pi2, right);
    }
    if (glfwGetKey(window, GLFW_KEY_KP_6))
    {
        camTransform->Translation() = { -10.0f, 0.0f, 0.0f };
        camTransform->Quaternion() = glm::angleAxis(pi + piHalf, up) * glm::angleAxis(pi2, right);
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