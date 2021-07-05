#include "Editor.h"

#include <glad/glad.h>
#include <glm/gtx/quaternion.hpp>
#include <stdio.h>

#include "Actions/ExtrudeNodeAction.h"
#include "Actions/InsertFaceAction.h"
#include "Actions/MoveNodeAction.h"
#include "Actions/MoveNodeHandleAction.h"
#include "Application.h"
#include "BezierCurveNode3.h"
#include "Camera.h"
#include "Gizmos.h"
#include "imgui.h"
#include "Object.h"
#include "RenderTexture.h"
#include "SelectionControl.h"
#include "Transform.h"
#include "Workspace.h"

Editor::Editor(Workspace* a_workspace)
{
    m_workspace = a_workspace;

    m_renderTexture = new RenderTexture(640, 480);

    m_editorMode = EditorMode_Object;
    m_faceCullingMode = EditorFaceCullingMode_Back;

    m_mouseDown = 0;

    m_camera = new Camera();
    m_camera->SetFOV(0.6911504f);

    m_curAction = nullptr;

    Transform* transform = m_camera->GetTransform();
    transform->Translation() = { 0.0f, -2.5f, -10.0f };
    transform->Quaternion() = glm::angleAxis(3.14159f, glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f))) * glm::angleAxis(3.14159f * 0.1f, glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f)));
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
void Editor::AddNodeToSelection(unsigned int a_index)
{
    for (auto iter = m_selectedNodes.begin(); iter != m_selectedNodes.end(); ++iter)
    {
        if (*iter == a_index)
        {
            return;
        }
    }

    m_selectedNodes.emplace_back(a_index);
}

void DrawCurve(int a_steps, const glm::mat4& a_modelMatrix, BezierCurveNode3& a_nodeA, BezierCurveNode3& a_nodeB)
{
    for (int i = 0; i < a_steps; ++i)
    {
        const glm::vec3 pointA = (a_modelMatrix * glm::vec4(BezierCurveNode3::GetPoint(a_nodeA, a_nodeB, (float)i / a_steps), 1)).xyz();
        const glm::vec3 pointB = (a_modelMatrix * glm::vec4(BezierCurveNode3::GetPoint(a_nodeA, a_nodeB, (float)(i + 1) / a_steps), 1)).xyz();

        Gizmos::DrawLine(pointA, pointB, 0.0025f, glm::vec4(0.93f, 0.53f, 0.00f, 1.00f));
    }
}

bool Editor::IsInteractingCurveNode(const glm::vec3& a_pos, const glm::vec3& a_axis, const glm::vec2& a_cursorPos, CurveModel* a_model, const glm::mat4& a_viewProj)
{
    if (SelectionControl::PointInPoint(a_viewProj, a_cursorPos, 0.05f, a_pos.xyz() + a_axis * 0.25f))
    {
        const unsigned int nodeCount = m_selectedNodes.size();
        unsigned int* indices = new unsigned int[nodeCount];

        unsigned int index = 0;
        for (auto iter = m_selectedNodes.begin(); iter != m_selectedNodes.end(); ++iter)
        {
            indices[index++] = *iter;
        }

        switch (m_workspace->GetToolMode())
        {
        case ToolMode_Translate:
        {
            m_curAction = new MoveNodeAction(m_workspace, indices, nodeCount, a_model, a_cursorPos, a_axis, a_axis);
            if (!m_workspace->PushAction(m_curAction))
            {
                printf("Error moving node");

                delete m_curAction;
                m_curAction = nullptr;
            }

            break;
        }
        case ToolMode_Extrude:
        {
            m_curAction = new ExtrudeNodeAction(m_workspace, this, indices, nodeCount, a_model, a_cursorPos, a_axis, a_axis);
            if (!m_workspace->PushAction(m_curAction))
            {
                printf("Error extruding node");

                delete m_curAction;
                m_curAction = nullptr;
            }

            break;
        }
        }
        
        delete[] indices;

        return true;
    }

    return false;
}
bool Editor::IsInteractingCurveNodeHandle(const Node3Cluster& a_node, unsigned int a_nodeIndex, CurveModel* a_model, const glm::mat4& a_viewProj, const glm::vec2& a_cursorPos, const glm::mat4& a_transform, const glm::vec3& a_up, const glm::vec3& a_right)
{
    for (auto nodeIter = a_node.Nodes.begin(); nodeIter != a_node.Nodes.end(); ++nodeIter)
    {
        if (SelectionControl::NodeHandleInPoint(a_viewProj, a_cursorPos, 0.05f, a_transform, *nodeIter))
        {
            m_curAction = new MoveNodeHandleAction(m_workspace, nodeIter - a_node.Nodes.begin(), a_nodeIndex, a_model, a_cursorPos, a_right, a_up);
            if (!m_workspace->PushAction(m_curAction))
            {
                printf("Error moving node handle");

                delete m_curAction;
                m_curAction = nullptr;
            }

            return true;
        }
    }

    return false;
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

                const Transform* camTransform = m_camera->GetTransform();

                const glm::mat4 camTransformMatrix = camTransform->ToMatrix();
                const glm::mat4 modelMatrix = obj->GetGlobalMatrix();

                const glm::vec3 camFor = glm::normalize(camTransformMatrix[2]);

                const int steps = curveModel->GetSteps();

                const CurveFace* faces = curveModel->GetFaces();
                const unsigned int faceCount = curveModel->GetFaceCount();

                const Node3Cluster* nodes = curveModel->GetNodes();
                const unsigned int nodeCount = curveModel->GetNodeCount();

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

                            DrawCurve(steps, modelMatrix, sNodes[FaceIndex_3Point_AB], sNodes[FaceIndex_3Point_BA]);
                            DrawCurve(steps, modelMatrix, sNodes[FaceIndex_3Point_AC], sNodes[FaceIndex_3Point_CA]);
                            DrawCurve(steps, modelMatrix, sNodes[FaceIndex_3Point_BC], sNodes[FaceIndex_3Point_CB]);

                            break;
                        }
                        case FaceMode_4Point:
                        {
                            BezierCurveNode3 sNodes[8];

                            for (int j = 0; j < 8; ++j)
                            {
                                sNodes[j] = nodes[face.Index[j]].Nodes[face.ClusterIndex[j]];
                            } 

                            DrawCurve(steps, modelMatrix, sNodes[FaceIndex_4Point_AB], sNodes[FaceIndex_4Point_BA]);
                            DrawCurve(steps, modelMatrix, sNodes[FaceIndex_4Point_BD], sNodes[FaceIndex_4Point_DB]);
                            DrawCurve(steps, modelMatrix, sNodes[FaceIndex_4Point_DC], sNodes[FaceIndex_4Point_CD]);
                            DrawCurve(steps, modelMatrix, sNodes[FaceIndex_4Point_CA], sNodes[FaceIndex_4Point_AC]);
                        }
                    }
                }

                for (unsigned int i = 0; i < nodeCount; ++i)
                {
                    bool selected = false;
                    for (auto iter = m_selectedNodes.begin(); iter != m_selectedNodes.end(); ++iter)
                    {
                        if (*iter == i)
                        {
                            selected = true;

                            break;
                        }
                    }

                    if (!selected)
                    {
                        const BezierCurveNode3 curve = nodes[i].Nodes[0];

                        const glm::vec4 pos = modelMatrix * glm::vec4(curve.GetPosition(), 1);

                        Gizmos::DrawCircleFilled(pos, camFor, 0.025f, 10, glm::vec4(0.61f, 0.35f, 0.02f, 1.00f));
                    }
                    else
                    {
                        const std::vector<BezierCurveNode3> nodeCluster = nodes[i].Nodes;
                        for (auto iter = nodeCluster.begin(); iter != nodeCluster.end(); ++iter)
                        {
                            const glm::vec4 pos = modelMatrix * glm::vec4(iter->GetPosition(), 1);

                            if (iter->GetHandlePosition().x != std::numeric_limits<float>().infinity())
                            {
                                const glm::vec4 handlePos = modelMatrix * glm::vec4(iter->GetHandlePosition(), 1);

                                Gizmos::DrawLine(pos, handlePos, camFor, 0.005f, glm::vec4(0.93f, 0.53f, 0.00f, 1.00f));
                                Gizmos::DrawCircleFilled(handlePos, camFor, 0.05f, 15, glm::vec4(0.93f, 0.53f, 0.00f, 1.00f));
                            }
                            else
                            {
                                Gizmos::DrawCircleFilled(pos, camFor, 0.05f, 15, glm::vec4(0.93f, 0.53f, 0.00f, 1.00f));
                            }
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

e_ActionType Editor::GetCurrentAction() const
{
    if (m_curAction != nullptr)
    {
        return m_curAction->GetActionType();
    }

    return ActionType_Null;
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

    if (!IsEditorModeEnabled(m_editorMode))
    {
        SetEditorMode(EditorMode_Object);
    }

    const glm::vec2 halfWinSize = a_winSize * 0.5f;

    Gizmos::Clear();

    const ImVec2 mousePos = io.MousePos; 
    const glm::vec2 cursorPos = glm::vec2(mousePos.x, mousePos.y);

    Transform* camTransform = m_camera->GetTransform();

    const glm::mat4 viewInv = camTransform->ToMatrix();
    const glm::mat4 view = glm::inverse(viewInv);

    const glm::mat4 proj = m_camera->GetProjection(a_winSize.x, a_winSize.y);
    const glm::mat4 projInv = glm::inverse(proj);

    const glm::mat4 viewProj = proj * view;

    const glm::quat camQuat = camTransform->Quaternion();
    const glm::mat4 camRotMatrix = glm::toMat4(camQuat);

    const glm::vec3 camForward = camRotMatrix[2].xyz();
    const glm::vec3 camUp = camRotMatrix[1].xyz();
    const glm::vec3 camRight = camRotMatrix[0].xyz();

    const glm::vec2 curCursorPos = (cursorPos - (a_winPos + halfWinSize)) / halfWinSize;

    if (ImGui::IsWindowHovered())
    {
        const Object* obj = m_workspace->GetSelectedObject();

        if (obj != nullptr)
        {
            CurveModel* model = obj->GetCurveModel();

            if (model != nullptr)
            {
                if (GetCurrentAction() != ActionType_InsertFace)
                {
                    if (glfwGetKey(window, GLFW_KEY_F))
                    {
                        const unsigned int indexCount = m_selectedNodes.size();

                        unsigned int* indices = new unsigned int[indexCount];

                        unsigned int index = 0;
                        for (auto iter = m_selectedNodes.begin(); iter != m_selectedNodes.end(); ++iter, ++index)
                        {
                            indices[index] = *iter;
                        }

                        m_curAction = new InsertFaceAction(m_workspace, indices, indexCount, model);
                        if (!m_workspace->PushAction(m_curAction))
                        {
                            delete m_curAction;
                            m_curAction = nullptr;
                        }

                        delete[] indices;
                    }
                }
                else if (!glfwGetKey(window, GLFW_KEY_F))
                {
                    m_curAction = nullptr;
                }

                if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
                {
                    if (!(m_mouseDown & 0b1 << 0))
                    {
                        m_startPos = curCursorPos;

                        if (m_curAction != nullptr)
                        {
                            switch (m_curAction->GetActionType())
                            {
                            case ActionType_MoveNodeHandle:
                            case ActionType_MoveNode:
                            {
                                m_curAction = nullptr;

                                break;
                            }
                            }
                        }

                        const Node3Cluster* nodes = model->GetNodes();

                        const glm::mat4 transformMat = obj->GetGlobalMatrix();

                        glm::vec3 pos = glm::vec3(0);

                        for (auto iter = m_selectedNodes.begin(); iter != m_selectedNodes.end(); ++iter)
                        {
                            pos += nodes[*iter].Nodes[0].GetPosition();
                        }

                        pos /= m_selectedNodes.size();

                        const glm::vec4 fPos = transformMat * glm::vec4(pos, 1);
                        if (!IsInteractingCurveNode(fPos.xyz(), glm::vec3(0, 0, 1), curCursorPos, model, viewProj) &&
                            !IsInteractingCurveNode(fPos.xyz(), glm::vec3(0, 1, 0), curCursorPos, model, viewProj) &&
                            !IsInteractingCurveNode(fPos.xyz(), glm::vec3(1, 0, 0), curCursorPos, model, viewProj))
                        {
                            for (auto iter = m_selectedNodes.begin(); iter != m_selectedNodes.end(); ++iter)
                            {
                                const unsigned int nodeIndex = *iter;

                                if (IsInteractingCurveNodeHandle(nodes[nodeIndex], nodeIndex, model, viewProj, curCursorPos, transformMat, camUp, camRight))
                                {
                                    break;
                                }
                            }
                        }                    
                    }

                    m_mouseDown |= 0b1 << 0;
                }
            }
        }
        
        if (ImGui::IsMouseDown(ImGuiMouseButton_Middle))
        {
            m_mouseDown |= 0b1 << 1;
        }   
    }

    if (m_mouseDown & 0b1 << 0 && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
    {
        m_mouseDown &= ~(0b1 << 0);

        const glm::vec2 min = glm::min(m_startPos, curCursorPos);
        const glm::vec2 max = glm::max(m_startPos, curCursorPos);

        switch (m_editorMode)
        {
            case EditorMode_Edit:
            {
                const std::list<Object*> selectedObjects = m_workspace->GetSelectedObjects();

                const Object* obj = *selectedObjects.begin();
                if (obj != nullptr)
                {
                    const glm::mat4 transformMat = obj->GetGlobalMatrix();

                    const CurveModel* curveModel = obj->GetCurveModel();

                    const unsigned int nodeCount = curveModel->GetNodeCount();
                    Node3Cluster* nodes = curveModel->GetNodes();

                    switch (GetCurrentAction())
                    {
                    case ActionType_ExtrudeNode:
                    case ActionType_MoveNode:
                    case ActionType_MoveNodeHandle:
                    {
                        m_curAction = nullptr;

                        break;
                    }
                    default:
                    {
                        if (io.KeyShift)
                        {
                            for (unsigned int i = 0; i < nodeCount; ++i)
                            {
                                const Node3Cluster node = nodes[i];
                                if (SelectionControl::NodeInSelection(viewProj, min, max, transformMat, node.Nodes[0]))
                                {
                                    m_selectedNodes.emplace_back(i);
                                }
                            }
                        }
                        else if (io.KeyCtrl)
                        {
                            for (unsigned int i = 0; i < nodeCount; ++i)
                            {
                                const Node3Cluster node = nodes[i];
                                if (SelectionControl::NodeInSelection(viewProj, min, max, transformMat, node.Nodes[0]))
                                {
                                    bool found = false;

                                    for (auto iter = m_selectedNodes.begin(); iter != m_selectedNodes.end(); ++iter)
                                    {
                                        if (*iter == i)
                                        {
                                            found = true;

                                            m_selectedNodes.erase(iter);

                                            break;
                                        }
                                    }

                                    if (!found)
                                    {
                                        m_selectedNodes.emplace_back(i);
                                    }
                                }
                            }
                        }
                        else
                        {
                            m_selectedNodes.clear();

                            for (unsigned int i = 0; i < nodeCount; ++i)
                            {
                                const Node3Cluster node = nodes[i];
                                if (SelectionControl::NodeInSelection(viewProj, min, max, transformMat, node.Nodes[0]))
                                {
                                    m_selectedNodes.emplace_back(i);
                                }
                            }
                        }

                        break;
                    }
                    }
                }
                
                
                break;
            }
        }
    }
    if (ImGui::IsMouseReleased(ImGuiMouseButton_Middle))
    {
        m_mouseDown &= ~(0b1 << 1);
    }

    if (m_mouseDown & 0b1 << 0)
    {
        switch (GetCurrentAction())
        {
        case ActionType_ExtrudeNode:
        {
            ExtrudeNodeAction* action = (ExtrudeNodeAction*)m_curAction;
            action->SetCursorPos(curCursorPos);

            action->Execute();

            break;
        }
        case ActionType_MoveNodeHandle:
        {
            MoveNodeHandleAction* action = (MoveNodeHandleAction*)m_curAction;
            action->SetCursorPos(curCursorPos);

            action->Execute();

            break;
        }
        case ActionType_MoveNode:
        {
            MoveNodeAction* action = (MoveNodeAction*)m_curAction;
            action->SetCursorPos(curCursorPos);

            action->Execute();

            break;
        }
        default:
        {
            // Dirty but cannot be stuffed making screen space gizmos
            const glm::vec2 min = glm::min(m_startPos, curCursorPos);
            const glm::vec2 max = glm::max(m_startPos, curCursorPos);

            const glm::vec4 sTL = glm::vec4(glm::vec2(min.x, min.y), -0.99f, 1.0f);
            const glm::vec4 sTR = glm::vec4(glm::vec2(max.x, min.y), -0.99f, 1.0f);
            const glm::vec4 sBL = glm::vec4(glm::vec2(min.x, max.y), -0.99f, 1.0f);
            const glm::vec4 sBR = glm::vec4(glm::vec2(max.x, max.y), -0.99f, 1.0f);

            glm::vec4 tlVP = projInv * sTL;
            tlVP /= tlVP.w;
            glm::vec4 trVP = projInv * sTR;
            trVP /= trVP.w;
            glm::vec4 blVP = projInv * sBL;
            blVP /= blVP.w;
            glm::vec4 brVP = projInv * sBR;
            brVP /= brVP.w;

            const glm::vec4 tlWP = viewInv * tlVP;
            const glm::vec4 trWP = viewInv * trVP;
            const glm::vec4 blWP = viewInv * blVP;
            const glm::vec4 brWP = viewInv * brVP;

            const glm::vec3 f = viewInv[2].xyz();

            Gizmos::DrawLine(tlWP.xyz(), trWP.xyz(), f, 0.0001f, glm::vec4(0.93f, 0.53f, 0.00f, 1.00f));
            Gizmos::DrawLine(trWP.xyz(), brWP.xyz(), f, 0.0001f, glm::vec4(0.93f, 0.53f, 0.00f, 1.00f));
            Gizmos::DrawLine(brWP.xyz(), blWP.xyz(), f, 0.0001f, glm::vec4(0.93f, 0.53f, 0.00f, 1.00f));
            Gizmos::DrawLine(blWP.xyz(), tlWP.xyz(), f, 0.0001f, glm::vec4(0.93f, 0.53f, 0.00f, 1.00f));

            break;
        }
        } 
    }

    camTransform->Translation() -= camForward * io.MouseWheel * 2.0f;

    if (m_mouseDown & 0b1 << 1)
    {
        glm::vec3 mov = glm::vec3(0);

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

        const glm::vec2 mouseMove = { io.MouseDelta.x, io.MouseDelta.y };

        const glm::vec2 camMov = mouseMove * 0.01f;

        if (mouseMove.x != -FLT_MAX && mouseMove.y != -FLT_MAX)
        {
            camTransform->Quaternion() = glm::angleAxis(-camMov.x, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::angleAxis(camMov.y, camRight) * camQuat;
        }
    }

    if (m_selectedNodes.size() > 0 && m_editorMode == EditorMode_Edit)
    {
        const std::list<Object*> selectedObjects = m_workspace->GetSelectedObjects();

        const Object* obj = *selectedObjects.begin();
        if (obj != nullptr)
        {
            const CurveModel* model = obj->GetCurveModel();
            const Node3Cluster* nodes = model->GetNodes();

            glm::vec3 pos = glm::vec3(0);

            for (auto iter = m_selectedNodes.begin(); iter != m_selectedNodes.end(); ++iter)
            {
                pos += nodes[*iter].Nodes[0].GetPosition();
            }

            pos /= m_selectedNodes.size();

            const glm::mat4 transformMat = obj->GetGlobalMatrix(); 

            const glm::vec4 fPos = transformMat * glm::vec4(pos, 1);

            Gizmos::DrawLine(fPos.xyz(), fPos.xyz() + glm::vec3(0.25f, 0, 0), viewInv[2].xyz(), 0.01f, glm::vec4(1, 0, 0, 1));
            Gizmos::DrawLine(fPos.xyz(), fPos.xyz() + glm::vec3(0, 0.25f, 0), viewInv[2].xyz(), 0.01f, glm::vec4(0, 1, 0, 1));
            Gizmos::DrawLine(fPos.xyz(), fPos.xyz() + glm::vec3(0, 0, 0.25f), viewInv[2].xyz(), 0.01f, glm::vec4(0, 0, 1, 1));
        }
    }

    const int width = m_renderTexture->GetWidth();
    const int height = m_renderTexture->GetHeight();

    const unsigned int handle = m_renderTexture->GetHandle();

    switch (m_faceCullingMode)
    {
        case EditorFaceCullingMode_Back:
        {
            glEnable(GL_CULL_FACE);  
            glCullFace(GL_BACK);  

            break;
        }
        case EditorFaceCullingMode_Front:
        {
            glEnable(GL_CULL_FACE);  
            glCullFace(GL_FRONT); 

            break;
        }
        case EditorFaceCullingMode_None:
        {
            glDisable(GL_CULL_FACE);

            break;
        }
        case EditorFaceCullingMode_All:
        {
            glEnable(GL_CULL_FACE);  
            glCullFace(GL_FRONT_AND_BACK); 

            break;
        }
    }
    
    glEnable(GL_DEPTH_TEST);  

    glm::vec4 viewCache;
    int fbCache;

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

    // TODO: Create transform Gizmos
    Gizmos::DrawLine(glm::vec3(0), glm::vec3(1, 0, 0), viewInv[2].xyz(), 0.01f, glm::vec4(1, 0, 0, 1));
    Gizmos::DrawLine(glm::vec3(0), glm::vec3(0, 1, 0), viewInv[2].xyz(), 0.01f, glm::vec4(0, 1, 0, 1));
    Gizmos::DrawLine(glm::vec3(0), glm::vec3(0, 0, 1), viewInv[2].xyz(), 0.01f, glm::vec4(0, 0, 1, 1));

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    Gizmos::DrawAll(m_camera, a_winSize);

    // Restore framebuffer state for imgui
    glBindFramebuffer(GL_FRAMEBUFFER, fbCache);
    glViewport(viewCache[0], viewCache[1], viewCache[2], viewCache[3]);

    glEnable(GL_CULL_FACE);  
}