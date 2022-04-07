#include "EditorControls/ShapeEditor.h"

#include <glad/glad.h>
#include <glm/gtx/quaternion.hpp>

#include "Actions/MoveShapeNodeAction.h"
#include "Actions/MoveShapeNodeHandleAction.h"
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
#include "ToolActions/ExtrudeShapeNodeToolAction.h"
#include "ToolActions/MoveShapeNodeToolAction.h"
#include "Transform.h"
#include "Workspace.h"

ShapeEditor::ShapeEditor(Workspace* a_workspace, Editor* a_editor)
{
    m_workspace = a_workspace;
    m_editor = a_editor;

    m_renderTexture = new RenderTexture(640, 480);

    m_currentAction = nullptr;

    m_camera = new Camera();
    m_camera->SetOrthographic(true);

    m_shapeActions = new ToolAction*[ToolMode_End];

    for (int i = 0; i < ToolMode_End; ++i)
    {
        m_shapeActions[i] = nullptr;
    }

    m_shapeActions[ToolMode_Translate] = new MoveShapeNodeToolAction(m_workspace, this);
    m_shapeActions[ToolMode_Extrude] = new ExtrudeShapeNodeToolAction(m_workspace, this);

    Init();
}
ShapeEditor::~ShapeEditor()
{
    for (int i = 0; i < ToolMode_End; ++i)
    {
        if (m_shapeActions[i] != nullptr)
        {
            delete m_shapeActions[i];
        }
    }
    delete[] m_shapeActions;

    delete m_camera;
    
    delete m_renderTexture;
}

void ShapeEditor::Init()
{
    constexpr float pi = glm::pi<float>();
    constexpr float halfPI = pi * 0.5f;

    constexpr glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f);

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

    Transform* camTrans = m_camera->GetTransform();
    camTrans->Translation() = { 0.0f, -2.5f, 0.0f };
    camTrans->Quaternion() = glm::angleAxis(halfPI, right);

    m_cameraZoom = 1.0f;
}

e_ActionType ShapeEditor::GetCurrentActionType() const
{
    if (m_currentAction != nullptr)
    {
        return m_currentAction->GetActionType();
    }

    return ActionType_Null;
}
void ShapeEditor::DrawSelection(const ShapeNodeCluster& a_node, unsigned int a_index) const
{
    constexpr glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    const glm::vec2 pos = a_node.Nodes[0].GetPosition();
    for (auto iter = m_selectedIndices.begin(); iter != m_selectedIndices.end(); ++iter)
    {
        if (*iter == a_index)
        {   
            const std::vector<BezierCurveNode2>& cNodes = a_node.Nodes;
            for (auto cIter = cNodes.begin(); cIter != cNodes.end(); ++cIter)
            {
                const glm::vec2 handle = cIter->GetHandlePosition();

                Gizmos::DrawLine(glm::vec3(pos.x, 0.0f, pos.y), glm::vec3(handle.x, 0.0f, handle.y), 0.01f, ColorTheme::Active);
                Gizmos::DrawCircleFilled(glm::vec3(handle.x, 0.0f, handle.y), up, 0.05f, 10, ColorTheme::Active);
            }
            
            return;
        }
    }

    Gizmos::DrawCircleFilled(glm::vec3(pos.x, 0.0f, pos.y), up, 0.05f, 10, ColorTheme::InActive);
}

bool ShapeEditor::MoveNodeHandle(const glm::mat4& a_viewProj, const ShapeNodeCluster& a_node, unsigned int a_index, const glm::vec2& a_cursorPos, PathModel* a_pathModel)
{
    const std::vector<BezierCurveNode2>& nodes = a_node.Nodes;
    for (auto iter = nodes.begin(); iter != nodes.end(); ++iter)
    {
        if (SelectionControl::NodeHandleInPoint(a_viewProj, a_cursorPos, 0.05f, *iter))
        {
            m_currentAction = new MoveShapeNodeHandleAction(m_workspace, a_index, iter - nodes.begin(), a_pathModel, a_cursorPos);
            if (!m_workspace->PushAction(m_currentAction))
            {
                printf("Error moving shape handle node \n");

                delete m_currentAction;
                m_currentAction = nullptr;
            }

            return true;
        }
    }

    

    return false;
}

unsigned int* ShapeEditor::GetSelectedIndicesArray() const
{
    const unsigned int size = m_selectedIndices.size();

    unsigned int* indices = new unsigned int[size];
    
    unsigned int index = 0;
    for (auto iter = m_selectedIndices.begin(); iter != m_selectedIndices.end(); ++iter)
    {
        indices[index++] = *iter;
    }

    return indices;
}

void ShapeEditor::AddNodeToSelection(unsigned int a_index)
{
    m_selectedIndices.emplace_back(a_index);
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

    Transform* camTrans = m_camera->GetTransform();

    const glm::mat4 viewInv = camTrans->ToMatrix();
    const glm::mat4 view = glm::inverse(viewInv);
    const glm::mat4 proj = m_camera->GetProjection(a_winSize * m_cameraZoom);
    const glm::mat4 viewProj = proj * view;

    const e_ToolMode toolMode = m_workspace->GetToolMode();

    if (m_mouseState & 0b1 << 0)
    {
        if (m_shapeActions[toolMode] == nullptr || !m_shapeActions[toolMode]->LeftDown(m_camera, curCursorPos, a_winSize))
        {
            switch (GetCurrentActionType())
            {
            case ActionType_MoveShapeNodeHandle:
            {
                MoveShapeNodeHandleAction *action = (MoveShapeNodeHandleAction *)m_currentAction;
                action->SetCursorPos(curCursorPos);

                action->Execute();

                break;
            }
            default:
            {
                const glm::vec2 min = glm::min(m_startCursorPos, curCursorPos);
                const glm::vec2 max = glm::max(m_startCursorPos, curCursorPos);

                const glm::vec3 tlWP = m_camera->GetScreenToWorld(glm::vec3(min.x, min.y, 0.0f), a_winSize * m_cameraZoom);
                const glm::vec3 trWP = m_camera->GetScreenToWorld(glm::vec3(max.x, min.y, 0.0f), a_winSize * m_cameraZoom);
                const glm::vec3 blWP = m_camera->GetScreenToWorld(glm::vec3(min.x, max.y, 0.0f), a_winSize * m_cameraZoom);
                const glm::vec3 brWP = m_camera->GetScreenToWorld(glm::vec3(max.x, max.y, 0.0f), a_winSize * m_cameraZoom);

                const glm::vec3 f = viewInv[2].xyz();

                Gizmos::DrawLine(tlWP, trWP, f, 0.005f, ColorTheme::Active);
                Gizmos::DrawLine(trWP, brWP, f, 0.005f, ColorTheme::Active);
                Gizmos::DrawLine(brWP, blWP, f, 0.005f, ColorTheme::Active);
                Gizmos::DrawLine(blWP, tlWP, f, 0.005f, ColorTheme::Active);

                break;
            }
            }
        }         
    }

    if (ImGui::IsWindowFocused())
    {
        m_cameraZoom -= io.MouseWheel * 0.025f;
        m_cameraZoom = glm::min(10.0f, glm::max(0.1f, m_cameraZoom));

        if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && !(m_mouseState & 0b1 << 0))
        {
            m_startCursorPos = curCursorPos;

            const Object* object = m_workspace->GetSelectedObject();
            if (object != nullptr)
            {
                PathModel* model = object->GetPathModel();
                if (model != nullptr)
                {
                    if (m_shapeActions[toolMode] == nullptr || !m_shapeActions[toolMode]->LeftClicked(m_camera, curCursorPos, a_winSize))
                    {
                        const ShapeNodeCluster* nodes = model->GetShapeNodes();

                        for (auto iter = m_selectedIndices.begin(); iter != m_selectedIndices.end(); ++iter)
                        {
                            const unsigned int index = *iter;

                            if (MoveNodeHandle(viewProj, nodes[index], index, curCursorPos, model))
                            {
                                break;
                            }
                        }
                    }
                }
            }

            m_mouseState |= 0b1 << 0;
        }

        if (ImGui::IsMouseDown(ImGuiMouseButton_Middle))
        {
            if (m_mouseState & 0b1 << 1)
            {
                camTrans->Translation().x += m_prevCursorPos.x - curCursorPos.x;
                camTrans->Translation().z += m_prevCursorPos.y - curCursorPos.y;
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
            const ShapeNodeCluster* nodes = model->GetShapeNodes();
            const unsigned int nodeCount = model->GetShapeNodeCount();

            if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) && m_mouseState & 0b1 << 0)
            {
                if (m_shapeActions[toolMode] == nullptr || !m_shapeActions[toolMode]->LeftReleased(m_camera, curCursorPos, a_winSize))
                {
                    switch (GetCurrentActionType())
                    {
                    case ActionType_MoveShapeNodeHandle:
                    {
                        m_currentAction = nullptr;

                        break;
                    }
                    default:
                    {
                        const glm::vec2 min = glm::min(m_startCursorPos, curCursorPos);
                        const glm::vec2 max = glm::max(m_startCursorPos, curCursorPos);

                        m_selectedIndices.clear();

                        for (unsigned int i = 0; i < nodeCount; ++i)
                        {
                            if (SelectionControl::NodeInSelection(viewProj, min, max, glm::identity<glm::mat4>(), nodes[i].Nodes[0]))
                            {
                                m_selectedIndices.emplace_back(i);
                            }
                        }

                        break;
                    }
                    }
                }
            
                m_mouseState &= ~(0b1 << 0);
            }

            const int shapeSteps = model->GetShapeSteps();

            const ShapeLine* lines = model->GetShapeLines();
            const unsigned int lineCount = model->GetShapeLineCount();

            for (unsigned int i = 0; i < lineCount; ++i)
            {
                const BezierCurveNode2& nodeA = nodes[lines[i].Index[0]].Nodes[lines[i].ClusterIndex[0]];
                const BezierCurveNode2& nodeB = nodes[lines[i].Index[1]].Nodes[lines[i].ClusterIndex[1]];

                for (int j = 0; j < shapeSteps; ++j)
                {
                    const glm::vec2 pointA = BezierCurveNode2::GetPoint(nodeA, nodeB, (j + 0) / (float)shapeSteps);
                    const glm::vec2 pointB = BezierCurveNode2::GetPoint(nodeA, nodeB, (j + 1) / (float)shapeSteps);

                    Gizmos::DrawLine(glm::vec3(pointA.x, 0.0f, pointA.y), glm::vec3(pointB.x, 0.0f, pointB.y), 0.0025f, ColorTheme::Active);
                }
            }

            for (unsigned int i = 0; i < nodeCount; ++i)
            {
                DrawSelection(nodes[i], i);
            }
        }
    }

    if (m_shapeActions[toolMode] != nullptr)
    {
        m_shapeActions[toolMode]->Draw(m_camera);
    }

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

    Gizmos::DrawAll(m_camera, a_winSize * m_cameraZoom);

    // Restore framebuffer state for imgui
    glBindFramebuffer(GL_FRAMEBUFFER, fbCache);
    glViewport(viewCache[0], viewCache[1], viewCache[2], viewCache[3]);
}