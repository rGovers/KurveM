#include "Editor.h"

#include <glad/glad.h>
#include <glm/gtx/quaternion.hpp>
#include <stdio.h>

#include "Application.h"
#include "BezierCurveNode3.h"
#include "Camera.h"
#include "CurveModel.h"
#include "Gizmos.h"
#include "imgui.h"
#include "Object.h"
#include "RenderTexture.h"
#include "Transform.h"
#include "Workspace.h"

Editor::Editor(Workspace* a_workspace)
{
    m_workspace = a_workspace;

    m_renderTexture = new RenderTexture(640, 480);

    m_editorMode = EditorMode_Object;

    m_mouseDown = false;

    m_camera = new Camera();
    m_camera->SetFOV(0.6911504f);

    Transform* transform = m_camera->GetTransform();
    transform->Translation() = { 0.0f, -2.5f, -5.0f };
    transform->Quaternion() = glm::angleAxis(3.14159f, glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f))) * glm::angleAxis(3.14159f * 0.25f, glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f)));
}
Editor::~Editor()
{
    delete m_renderTexture;

    delete m_camera;
}

bool Editor::IsEditorModeEnabled(e_EditorMode a_editorMode) const
{
    switch (a_editorMode)
    {
        case EditorMode_Edit:
        {
            const std::list<Object*> selectedObjs = m_workspace->GetSelectedObjects();

            return selectedObjs.size() == 1;
        }
        case EditorMode_End:
        {
            return false;
        }
    }

    return true;
}

void DrawCurveHandle(int a_steps, BezierCurveNode3& a_nodeA, BezierCurveNode3& a_nodeB)
{
    for (int i = 0; i < a_steps; ++i)
    {
        const glm::vec3 pointA = BezierCurveNode3::GetPoint(a_nodeA, a_nodeB, (float)i / a_steps);
        const glm::vec3 pointB = BezierCurveNode3::GetPoint(a_nodeA, a_nodeB, (float)(i + 1) / a_steps);

        Gizmos::DrawLine(pointA, pointB, 0.001f, glm::vec4(0.93f, 0.53f, 0.00f, 1.00f));
    }

    Gizmos::DrawLine(a_nodeA.Position(), a_nodeA.HandlePosition(), 0.0025f, glm::vec4(0.61f, 0.35f, 0.00f, 1.00f));
    Gizmos::DrawLine(a_nodeB.Position(), a_nodeB.HandlePosition(), 0.0025f, glm::vec4(0.61f, 0.35f, 0.00f, 1.00f));
}

void Editor::DrawObject(Object* a_object, const glm::vec2& a_winSize)
{
    a_object->Draw(m_camera, a_winSize);

    switch (m_editorMode)
    {
        case EditorMode_Edit:
        {
            const std::list<Object*> selectedObjects = m_workspace->GetSelectedObjects();

            if (selectedObjects.size() != 1)
            {
                break;
            }

            Object* obj = *selectedObjects.begin();

            if (obj == a_object)
            {
                const CurveModel* curveModel = a_object->GetCurveModel();

                const int steps = curveModel->GetSteps();

                const CurveFace* faces = curveModel->GetFaces();
                const unsigned int faceCount = curveModel->GetFaceCount();

                const Node3Cluster* nodes = curveModel->GetNodes();

                for (unsigned int i = 0; i < faceCount; ++i)
                {
                    const CurveFace face = faces[i];

                    switch (face.FaceMode)
                    {
                        case FaceMode_3Point:
                        {
                            BezierCurveNode3 sNodes[6];

                            for (int j = 0; j < 6; ++j)
                            {
                                sNodes[j] = nodes[face.Index[j]].Nodes[face.ClusterIndex[j]];
                            } 

                            DrawCurveHandle(steps, sNodes[FaceIndex_3Point_AB], sNodes[FaceIndex_3Point_BA]);
                            DrawCurveHandle(steps, sNodes[FaceIndex_3Point_AC], sNodes[FaceIndex_3Point_CA]);
                            DrawCurveHandle(steps, sNodes[FaceIndex_3Point_BC], sNodes[FaceIndex_3Point_CB]);

                            break;
                        }
                        case FaceMode_4Point:
                        {
                            BezierCurveNode3 sNodes[8];

                            for (int j = 0; j < 8; ++j)
                            {
                                sNodes[j] = nodes[face.Index[j]].Nodes[face.ClusterIndex[j]];
                            } 

                            DrawCurveHandle(steps, sNodes[FaceIndex_4Point_AB], sNodes[FaceIndex_4Point_BA]);
                            DrawCurveHandle(steps, sNodes[FaceIndex_4Point_BD], sNodes[FaceIndex_4Point_DB]);
                            DrawCurveHandle(steps, sNodes[FaceIndex_4Point_DC], sNodes[FaceIndex_4Point_CD]);
                            DrawCurveHandle(steps, sNodes[FaceIndex_4Point_CA], sNodes[FaceIndex_4Point_AC]);
                        }
                    }
                }
            }

            break;
        }
    }

    const std::list<Object*> children = a_object->GetChildren();

    for (auto iter = children.begin(); iter != children.end(); ++iter)
    {
        DrawObject(*iter, a_winSize);
    }
}

void Editor::Update(double a_delta, const glm::vec2& a_winPos, const glm::vec2& a_winSize)
{
    if (a_winSize.x != m_renderTexture->GetWidth() || a_winSize.y != m_renderTexture->GetHeight())
    {
        m_renderTexture->Resize(a_winSize.x, a_winSize.y);
    }

    const Application* app = Application::GetInstance();
    GLFWwindow* window = app->GetWindow();
    const ImGuiIO& io = ImGui::GetIO();

    Gizmos::Clear();

    if (ImGui::IsWindowHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Middle))
    {
        m_mouseDown = true;
    }
    else if (ImGui::IsMouseReleased(ImGuiMouseButton_Middle))
    {
        m_mouseDown = false;
    }

    if (m_mouseDown)
    {
        Transform* transform = m_camera->GetTransform();

        const glm::quat quat = transform->Quaternion();
        const glm::mat4 rotMat = glm::toMat4(quat);

        const glm::vec3 forward = rotMat[2].xyz();
        const glm::vec3 up = rotMat[1].xyz();
        const glm::vec3 right = rotMat[0].xyz();

        glm::vec3 mov = glm::vec3(0);

        if (glfwGetKey(window, GLFW_KEY_W))
        {
            mov -= forward;
        }
        if (glfwGetKey(window, GLFW_KEY_S))
        {
            mov += forward;
        }
        if (glfwGetKey(window, GLFW_KEY_A))
        {
            mov -= right;
        }
        if (glfwGetKey(window, GLFW_KEY_D))
        {
            mov += right;
        }

        transform->Translation() += mov * 2.0f * (float)a_delta;

        const glm::vec2 mouseMove = { io.MouseDelta.x, io.MouseDelta.y };

        const glm::vec2 camMov = mouseMove * 0.01f;

        if (mouseMove.x != -FLT_MAX && mouseMove.y != -FLT_MAX)
        {
            transform->Quaternion() = glm::angleAxis(-camMov.x, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::angleAxis(camMov.y, right) * quat;
        }
    }

    if (!IsEditorModeEnabled(m_editorMode))
    {
        SetEditorMode(EditorMode_Object);
    }

    const int width = m_renderTexture->GetWidth();
    const int height = m_renderTexture->GetHeight();

    const unsigned int handle = m_renderTexture->GetHandle();

    glm::vec4 viewCache;
    int fbCache;

    glEnable(GL_CULL_FACE);  
    glCullFace(GL_BACK);  
    
    glEnable(GL_DEPTH_TEST);  

    glGetFloatv(GL_VIEWPORT, (float*)&viewCache);
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint*)&fbCache);

    glViewport(0, 0, width, height);

    glBindFramebuffer(GL_FRAMEBUFFER, handle);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    glClearColor(163.0f / 255, 163.0f / 255, 162.0f / 255, 1);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);    

    const std::list<Object*> objs = m_workspace->GetObjectList();

    for (auto iter = objs.begin(); iter != objs.end(); ++iter)
    {
        Object* obj = *iter;

        DrawObject(obj, a_winSize);
    }

    Gizmos::DrawLine(glm::vec3(0), glm::vec3(1, 0, 0), 0.01f, glm::vec4(1, 0, 0, 1));
    Gizmos::DrawLine(glm::vec3(0), glm::vec3(0, 1, 0), 0.01f, glm::vec4(0, 1, 0, 1));
    Gizmos::DrawLine(glm::vec3(0), glm::vec3(0, 0, 1), 0.01f, glm::vec4(0, 0, 1, 1));

    Gizmos::DrawAll(m_camera, a_winSize);

    glBindFramebuffer(GL_FRAMEBUFFER, fbCache);

    glViewport(viewCache[0], viewCache[1], viewCache[2], viewCache[3]);
}