#include "EditorControls/ShapeEditor.h"

#include <glad/glad.h>
#include <glm/gtx/quaternion.hpp>

#include "Camera.h"
#include "ColorTheme.h"
#include "Datastore.h"
#include "Gizmos.h"
#include "imgui.h"
#include "Model.h"
#include "PathModel.h"
#include "RenderTexture.h"
#include "SelectionControl.h"
#include "ShaderPixel.h"
#include "ShaderProgram.h"
#include "Shaders/GridPixel.h"
#include "Shaders/GridVertex.h"
#include "ShaderVertex.h"
#include "Transform.h"
#include "Workspace.h"

ShapeEditor::ShapeEditor(Workspace* a_workspace, Editor* a_editor)
{
    m_workspace = a_workspace;
    m_editor = a_editor;

    m_renderTexture = new RenderTexture(640, 480);

    m_currentAction = nullptr;

    m_camera = new Camera();

    Init();
}
ShapeEditor::~ShapeEditor()
{
    delete m_camera;
    
    delete m_renderTexture;
}

void ShapeEditor::Init()
{
    constexpr float halfPI = glm::pi<float>() * 0.5f;

    m_gridShader = Datastore::GetShaderProgram("SHADER_GRID");
    if (m_gridShader == nullptr)
    {   
        ShaderVertex* vertexShader = new ShaderVertex(GRIDVERTEX);
        ShaderPixel* pixelShader = new ShaderPixel(GRIDPIXEL);

        m_gridShader = new ShaderProgram(vertexShader, pixelShader);

        delete vertexShader;
        delete pixelShader;

        Datastore::AddShaderProgram("SHADER_GRID", m_gridShader);
    }

    Transform* transform = m_camera->GetTransform();
    transform->Translation() = { 0.0f, -2.5f, 0.0f };
    transform->Quaternion() = glm::angleAxis(halfPI, glm::vec3(1.0f, 0.0f, 0.0f));
}

e_ActionType ShapeEditor::GetCurrentAction() const
{
    if (m_currentAction != nullptr)
    {
        return m_currentAction->GetActionType();
    }

    return ActionType_Null;
}
void ShapeEditor::DrawSelection(const BezierCurveNode2& a_node, unsigned int a_index) const
{
    const glm::vec2 pos = a_node.GetPosition();
    for (auto iter = m_selectedIndices.begin(); iter != m_selectedIndices.end(); ++iter)
    {
        if (*iter == a_index)
        {
            const glm::vec2 handle = a_node.GetHandlePosition();

            Gizmos::DrawLine(glm::vec3(pos.x, 0.0f, pos.y), glm::vec3(handle.x, 0.0f, handle.y), 0.01f, ColorTheme::Active);
            Gizmos::DrawCircleFilled(glm::vec3(handle.x, 0.0f, handle.y), glm::vec3(0.0f, 1.0f, 0.0f), 0.05f, 10, ColorTheme::Active);

            return;
        }
    }

    Gizmos::DrawCircleFilled(glm::vec3(pos.x, 0.0f, pos.y), glm::vec3(0.0f, 1.0f, 0.0f), 0.05f, 10, ColorTheme::InActive);
}

void ShapeEditor::ClearSelectedNodes()
{
    m_selectedIndices.clear();
}

void ShapeEditor::Update(double a_delta, const glm::vec2& a_winPos, const glm::vec2& a_winSize)
{
    if (a_winSize.x != m_renderTexture->GetWidth() || a_winSize.y != m_renderTexture->GetHeight())
    {
        m_renderTexture->Resize(a_winSize.x, a_winSize.y);
    }

    const ImGuiIO& io = ImGui::GetIO();

    const glm::vec2 halfWinSize = a_winSize * 0.5f;

    const ImVec2 mousePos = io.MousePos; 
    const glm::vec2 cursorPos = glm::vec2(mousePos.x, mousePos.y);

    const glm::vec2 curCursorPos = (cursorPos - (a_winPos + halfWinSize)) / halfWinSize;

    glm::vec4 viewCache;
    int fbCache;

    glGetFloatv(GL_VIEWPORT, (float*)&viewCache);
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint*)&fbCache);

    const int width = m_renderTexture->GetWidth();
    const int height = m_renderTexture->GetHeight();

    const unsigned int handle = m_renderTexture->GetHandle();

    glViewport(0, 0, width, height);

    glBindFramebuffer(GL_FRAMEBUFFER, handle);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);  
    glCullFace(GL_BACK);  

    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  

    Gizmos::Clear();

    Transform* transform = m_camera->GetTransform();

    const glm::mat4 viewInv = transform->ToMatrix();
    const glm::mat4 view = glm::inverse(viewInv);
    const glm::mat4 proj = m_camera->GetProjection(a_winSize);

    if (m_mouseState & 0b1 << 0)
    {
        const glm::vec2 min = glm::min(m_startCursorPos, curCursorPos);
        const glm::vec2 max = glm::max(m_startCursorPos, curCursorPos);

        const glm::vec3 tlWP = m_camera->GetScreenToWorld(glm::vec3(min.x, min.y, -0.99f), a_winSize);
        const glm::vec3 trWP = m_camera->GetScreenToWorld(glm::vec3(max.x, min.y, -0.99f), a_winSize);
        const glm::vec3 blWP = m_camera->GetScreenToWorld(glm::vec3(min.x, max.y, -0.99f), a_winSize);
        const glm::vec3 brWP = m_camera->GetScreenToWorld(glm::vec3(max.x, max.y, -0.99f), a_winSize);

        const glm::vec3 f = viewInv[2].xyz();

        // Gizmos::DrawLine(glm::vec3(1, 0, 0), glm::vec3(-1, 0, 0), f, 0.01f, ColorTheme::Active);

        Gizmos::DrawLine(tlWP, trWP, f, 0.0005f, ColorTheme::Active);
        Gizmos::DrawLine(trWP, brWP, f, 0.0005f, ColorTheme::Active);
        Gizmos::DrawLine(brWP, blWP, f, 0.0005f, ColorTheme::Active);
        Gizmos::DrawLine(blWP, tlWP, f, 0.0005f, ColorTheme::Active);
    }

    if (ImGui::IsWindowFocused())
    {
        transform->Translation().y += io.MouseWheel * 0.25f;
        if (transform->Translation().y >= -0.5f)
        {
            transform->Translation().y = -0.5f;
        }

        if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && !(m_mouseState & 0b1 << 0))
        {
            m_startCursorPos = curCursorPos;

            m_mouseState |= 0b1 << 0;
        }

        if (ImGui::IsMouseDown(ImGuiMouseButton_Middle))
        {
            if (m_mouseState & 0b1 << 1)
            {
                transform->Translation().x += m_prevCursorPos.x - curCursorPos.x;
                transform->Translation().z += m_prevCursorPos.y - curCursorPos.y;
            }

            m_mouseState |= 0b1 << 1;
            m_prevCursorPos = curCursorPos;
        }
    }

    if (ImGui::IsMouseReleased(ImGuiMouseButton_Middle))
    {
        m_mouseState &= ~(0b1 << 1);
    }

    const Object* object = m_workspace->GetSelectedObject();
    if (object != nullptr)
    {
        PathModel* model = object->GetPathModel();
        if (model != nullptr)
        {
            if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) && m_mouseState & 0b1 << 0)
            {
                const BezierCurveNode2* nodes = model->GetShapeNodes();
                const unsigned int nodeCount = model->GetShapeNodeCount();

                const glm::vec2 min = glm::min(m_startCursorPos, curCursorPos);
                const glm::vec2 max = glm::max(m_startCursorPos, curCursorPos);

                m_selectedIndices.clear();

                for (unsigned int i = 0; i < nodeCount; ++i)
                {
                    if (SelectionControl::NodeInSelection(proj * view, min, max, glm::identity<glm::mat4>(), nodes[i]))
                    {
                        m_selectedIndices.emplace_back(i);
                    }
                }
            
                m_mouseState &= ~(0b1 << 0);
            }

            const int shapeSteps = model->GetShapeSteps();

            const BezierCurveNode2* nodes = model->GetShapeNodes();
            const unsigned int* indices = model->GetPathIndices();
            const unsigned int indexCount = model->GetPathIndexCount();
            const unsigned int lineCount = indexCount / 2;

            for (unsigned int i = 0; i < lineCount; ++i)
            {
                const unsigned int indexA = indices[i * 2 + 0U];
                const unsigned int indexB = indices[i * 2 + 1U];

                const BezierCurveNode2 nodeA = nodes[indexA];
                const BezierCurveNode2 nodeB = nodes[indexB];

                DrawSelection(nodeA, indexA);
                DrawSelection(nodeB, indexB);

                for (int j = 0; j < shapeSteps; ++j)
                {
                    const glm::vec2 pointA = BezierCurveNode2::GetPoint(nodeA, nodeB, (j + 0) / (float)shapeSteps);
                    const glm::vec2 pointB = BezierCurveNode2::GetPoint(nodeA, nodeB, (j + 1) / (float)shapeSteps);

                    Gizmos::DrawLine(glm::vec3(pointA.x, 0.0f, pointA.y), glm::vec3(pointB.x, 0.0f, pointB.y), 0.0025f, ColorTheme::Active);
                }
            }
        }
    }

    Gizmos::DrawAll(m_camera, a_winSize);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);  
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);  

    const unsigned int programHandle = m_gridShader->GetHandle();
    glUseProgram(programHandle);

    glBindVertexArray(Model::GetEmpty()->GetVAO());

    glUniformMatrix4fv(0, 1, false, (float*)&view);
    glUniformMatrix4fv(1, 1, false, (float*)&proj);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glDisable(GL_BLEND);

    // Restore framebuffer state for imgui
    glBindFramebuffer(GL_FRAMEBUFFER, fbCache);
    glViewport(viewCache[0], viewCache[1], viewCache[2], viewCache[3]);
}