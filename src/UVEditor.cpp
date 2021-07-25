#include "UVEditor.h"

#include <glad/glad.h>

#include "Actions/MoveUVAction.h"
#include "ColorTheme.h"
#include "CurveModel.h"
#include "Editor.h"
#include "Gizmos.h"
#include "imgui.h"
#include "Object.h"
#include "RenderTexture.h"
#include "SelectionControl.h"
#include "Workspace.h"

const float BorderSize = 0.9f;

UVEditor::UVEditor(Workspace* a_workspace, Editor* a_editor)
{
    m_workspace = a_workspace;
    m_editor = a_editor;

    m_renderTexture = new RenderTexture(640, 480);

    m_currentAction = nullptr;
}
UVEditor::~UVEditor()
{
    delete m_renderTexture;
}

void UVEditor::MoveUV(const glm::vec2& a_pos, const glm::vec2& a_cursorPos, const glm::vec2& a_axis, CurveModel* a_curveModel)
{
    if (SelectionControl::PointInPoint(a_pos + a_axis * 0.1f, a_cursorPos, 0.01f))
    {
        const std::list<unsigned int> selectedNodes = m_editor->GetSelectedNodes();
        const unsigned int nodeCount = selectedNodes.size();

        unsigned int* indices = new unsigned int[nodeCount];
        unsigned int index = 0;

        for (auto iter = selectedNodes.begin(); iter != selectedNodes.end(); ++iter)
        {
            indices[index++] = *iter;
        }

        m_currentAction = new MoveUVAction(m_workspace, indices, nodeCount, a_curveModel, a_cursorPos * 0.5f + 0.5f, a_axis);
        if (!m_workspace->PushAction(m_currentAction))
        {
            printf("Error moving uv \n");

            delete m_currentAction;
            m_currentAction = nullptr;
        }
    }
}

e_ActionType UVEditor::GetCurrentAction() const
{
    if (m_currentAction != nullptr)
    {
        return m_currentAction->GetActionType();
    }

    return ActionType_Null;
}

void UVEditor::Update(double a_delta, const glm::vec2& a_winPos, const glm::vec2& a_winSize)
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

    glClearColor(0.3f, 0.3f, 0.3f, 1);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  

    Gizmos::Clear();

    for (int i = 0; i <= 10; ++i)
    {
        const float pos = (i / 10.0f * 2 - 1) * BorderSize;

        Gizmos::DrawLine(glm::vec3(pos, -BorderSize, 0), glm::vec3(pos, BorderSize, 0), glm::vec3(0, 0, 1), 0.001f, ColorTheme::InActive);
        Gizmos::DrawLine(glm::vec3(-BorderSize, pos, 0), glm::vec3(BorderSize, pos, 0), glm::vec3(0, 0, 1), 0.001f, ColorTheme::InActive);
    }

    Object* obj = m_workspace->GetSelectedObject();
    if (obj != nullptr)
    {
        CurveModel* model = obj->GetCurveModel();
        if (model != nullptr)
        {
            const std::list<unsigned int> selectedNodes = m_editor->GetSelectedNodes();
            const unsigned int nodeCount = selectedNodes.size();
            const Node3Cluster* nodes = model->GetNodes();

            if (nodeCount > 0)
            {   
                glm::vec2 pos = glm::vec2(0);
                for (auto iter = selectedNodes.begin(); iter != selectedNodes.end(); ++iter)
                {
                    const Node3Cluster node = nodes[*iter];

                    const glm::vec2 uv = node.Nodes[0].Node.GetUV();

                    pos += uv;

                    Gizmos::DrawCircleFilled(glm::vec3((uv * 2.0f - 1.0f) * BorderSize, 0.1f), glm::vec3(0, 0, 1), 0.01f, 10, ColorTheme::Active);
                }

                pos /= nodeCount;

                const glm::vec2 clipPos = (pos * 2.0f - 1.0f) * BorderSize;

                Gizmos::DrawLine(glm::vec3(clipPos, 0.2f), glm::vec3(clipPos + glm::vec2(0.1f, 0), 0.2f), glm::vec3(0, 0, 1), 0.005f, glm::vec4(1, 0, 0, 1));
                Gizmos::DrawLine(glm::vec3(clipPos, 0.2f), glm::vec3(clipPos + glm::vec2(0, 0.1f), 0.2f), glm::vec3(0, 0, 1), 0.005f, glm::vec4(0, 1, 0, 1));

                if (ImGui::IsWindowFocused())
                {
                    if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
                    {
                        if (!(m_mouseDown & 0b1 << 0))
                        {
                            switch (GetCurrentAction())
                            {
                            case ActionType_MoveUVAction:
                            {
                                m_currentAction = nullptr;

                                break;
                            }
                            }

                            switch (m_workspace->GetToolMode())
                            {
                            case ToolMode_Translate:
                            {
                                MoveUV(clipPos, curCursorPos, glm::vec2(1, 0), model);
                                MoveUV(clipPos, curCursorPos, glm::vec2(0, 1), model);

                                break;
                            }
                            }
                        }

                        m_mouseDown |= 0b1 << 0;
                    }
                }
            }
        }
    }

    if (m_mouseDown & 0b1 << 0)
    {
        switch (GetCurrentAction())
        {
        case ActionType_MoveUVAction:
        {
            MoveUVAction* action = (MoveUVAction*)m_currentAction;

            action->SetPosition(curCursorPos * 0.5f + 0.5f);
            action->Execute();

            break;
        }
        }
    }

    if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
    {
        switch (GetCurrentAction())
        {
        case ActionType_MoveUVAction:
        {
            m_currentAction = nullptr;

            break;
        }
        }

        m_mouseDown &= ~(0b1 << 0);
    }

    Gizmos::DrawAll();

    // Restore framebuffer state for imgui
    glBindFramebuffer(GL_FRAMEBUFFER, fbCache);
    glViewport(viewCache[0], viewCache[1], viewCache[2], viewCache[3]);
}