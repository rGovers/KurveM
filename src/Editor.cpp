#include "Editor.h"

#include <glad/glad.h>
#include <glm/gtx/quaternion.hpp>
#include <stdio.h>

#include "Actions/ExtrudeNodeAction.h"
#include "Actions/FlipFaceAction.h"
#include "Actions/InsertFaceAction.h"
#include "Actions/MoveNodeAction.h"
#include "Actions/MoveNodeHandleAction.h"
#include "Actions/TranslateObjectRelativeAction.h"
#include "Application.h"
#include "BezierCurveNode3.h"
#include "Camera.h"
#include "Datastore.h"
#include "Gizmos.h"
#include "imgui.h"
#include "Modals/DeleteNodesModal.h"
#include "Model.h"
#include "Object.h"
#include "RenderTexture.h"
#include "ShaderPixel.h"
#include "ShaderProgram.h"
#include "Shaders/GridPixel.h"
#include "Shaders/GridVertex.h"
#include "ShaderVertex.h"
#include "SelectionControl.h"
#include "Transform.h"
#include "Workspace.h"

Editor::Editor(Workspace* a_workspace)
{
    m_workspace = a_workspace;
    
    m_renderTexture = new RenderTexture(640, 480);

    m_mouseDown = 0;

    m_camera = new Camera();
    m_camera->SetFOV(0.6911504f);
    
    Init();
}
Editor::~Editor()
{
    delete m_renderTexture;

    delete m_camera;
}

void Editor::Init()
{
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
    transform->Translation() = { 0.0f, -2.5f, -10.0f };
    transform->Quaternion() = glm::angleAxis(3.14159f, glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f))) * glm::angleAxis(3.14159f * 0.1f, glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f)));

    m_curAction = nullptr;

    m_editorMode = EditorMode_Object;
    m_faceCullingMode = EditorFaceCullingMode_Back;
}

bool Editor::IsFaceSelected() const
{
    const Object* obj = m_workspace->GetSelectedObject();
    if (obj != nullptr)
    {
        const CurveModel* curveModel = obj->GetCurveModel();
        if (curveModel != nullptr)
        {
            const unsigned int nodeCount = m_selectedNodes.size();

            unsigned int* nodes = new unsigned int[nodeCount];

            unsigned int index = 0;
            for (auto iter = m_selectedNodes.begin(); iter != m_selectedNodes.end(); ++iter)
            {
                nodes[index++] = *iter;
            }

            bool ret = false;

            switch (nodeCount)
            {
            case 3:
            {
                ret = curveModel->Get3PointFaceIndex(nodes[0], nodes[1], nodes[2]) != -1;

                break;
            }
            case 6:
            {
                ret = curveModel->Get3PointFaceIndex(nodes) != -1;

                break;
            }
            case 4:
            {
                ret = curveModel->Get4PointFaceIndex(nodes[0], nodes[1], nodes[2], nodes[3]) != -1;

                break;
            }
            case 8:
            {
                ret = curveModel->Get4PointFaceIndex(nodes) != -1;

                break;
            }
            }

            delete[] nodes;

            return ret;
        }
    }

    return false;
}
bool Editor::CanInsertFace() const
{
    const Object* obj = m_workspace->GetSelectedObject();
    if (obj != nullptr)
    {
        const CurveModel* curveModel = obj->GetCurveModel();
        if (curveModel != nullptr)
        {
            const unsigned int nodeCount = m_selectedNodes.size();

            return nodeCount == 3 || nodeCount == 4;
        }
    }

    return false;
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

bool Editor::IsInteractingTransform(const glm::vec3& a_pos, const glm::vec3& a_axis, const glm::vec2& a_cursorPos, const glm::vec2& a_screenSize, const glm::mat4& a_viewProj)
{
    if (SelectionControl::PointInPoint(a_viewProj, a_cursorPos, 0.05f, a_pos + a_axis * 0.25f))
    {
        const glm::vec3 cPos = m_camera->GetScreenToWorld(glm::vec3(a_cursorPos, 0.9f), (int)a_screenSize.x, (int)a_screenSize.y);

        const unsigned int objectCount = m_workspace->GetSelectedObjectCount();
        Object** objs = m_workspace->GetSelectedObjectArray();

        switch (m_workspace->GetToolMode())
        {
        case ToolMode_Translate:
        {
            m_curAction = new TranslateObjectRelativeAction(cPos, a_axis, objs, objectCount);
            if (!m_workspace->PushAction(m_curAction))
            {
                printf("Error moving object \n");

                delete m_curAction;
                m_curAction = nullptr;
            }

            break;
        }
        }

        delete[] objs;
    }
}
bool Editor::IsInteractingCurveNode(const glm::vec3& a_pos, const glm::vec3& a_axis, const glm::vec2& a_cursorPos, const glm::vec2& a_screenSize, CurveModel* a_model, const glm::mat4& a_viewProj)
{
    if (SelectionControl::PointInPoint(a_viewProj, a_cursorPos, 0.05f, a_pos + a_axis * 0.25f))
    {
        const glm::vec3 cPos = m_camera->GetScreenToWorld(glm::vec3(a_cursorPos, 0.9f), (int)a_screenSize.x, (int)a_screenSize.y);

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
            m_curAction = new MoveNodeAction(m_workspace, indices, nodeCount, a_model, cPos, a_axis);
            if (!m_workspace->PushAction(m_curAction))
            {
                printf("Error moving node \n");

                delete m_curAction;
                m_curAction = nullptr;
            }

            break;
        }
        case ToolMode_Extrude:
        {
            m_curAction = new ExtrudeNodeAction(m_workspace, this, indices, nodeCount, a_model, cPos, a_axis);
            if (!m_workspace->PushAction(m_curAction))
            {
                printf("Error extruding node \n");

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
        if (SelectionControl::NodeHandleInPoint(a_viewProj, a_cursorPos, 0.05f, a_transform, nodeIter->Node))
        {
            m_curAction = new MoveNodeHandleAction(m_workspace, nodeIter - a_node.Nodes.begin(), a_nodeIndex, a_model, a_cursorPos, a_right, a_up);
            if (!m_workspace->PushAction(m_curAction))
            {
                printf("Error moving node handle \n");

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
            Object* obj = m_workspace->GetSelectedObject();

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
                                sNodes[j] = nodes[face.Index[j]].Nodes[face.ClusterIndex[j]].Node;
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
                                sNodes[j] = nodes[face.Index[j]].Nodes[face.ClusterIndex[j]].Node;
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
                        const BezierCurveNode3 curve = nodes[i].Nodes[0].Node;

                        const glm::vec4 pos = modelMatrix * glm::vec4(curve.GetPosition(), 1);

                        Gizmos::DrawCircleFilled(pos, camFor, 0.025f, 10, glm::vec4(0.61f, 0.35f, 0.02f, 1.00f));
                    }
                    else
                    {
                        const std::vector<NodeGroup> nodeCluster = nodes[i].Nodes;
                        for (auto iter = nodeCluster.begin(); iter != nodeCluster.end(); ++iter)
                        {
                            const glm::vec4 pos = modelMatrix * glm::vec4(iter->Node.GetPosition(), 1);

                            if (iter->Node.GetHandlePosition().x != std::numeric_limits<float>().infinity())
                            {
                                const glm::vec4 handlePos = modelMatrix * glm::vec4(iter->Node.GetHandlePosition(), 1);

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

    if (ImGui::IsWindowFocused())
    {
        camTransform->Translation() -= camForward * io.MouseWheel * 2.0f;

        const Object* obj = m_workspace->GetSelectedObject();

        if (obj != nullptr)
        {
            CurveModel* model = obj->GetCurveModel();

            if (model != nullptr)
            {
                if (glfwGetKey(window, GLFW_KEY_DELETE))
                {
                    if (!m_deleteDown)
                    {
                        const unsigned int size = m_selectedNodes.size();
                        if (size > 0)
                        {
                            unsigned int* indices = new unsigned int[size];

                            unsigned int index = 0;
                            for (auto iter = m_selectedNodes.begin(); iter != m_selectedNodes.end(); ++iter)
                            {
                                indices[index++] = *iter;
                            }

                            m_workspace->PushModal(new DeleteNodesModal(m_workspace, this, indices, size, model));

                            delete[] indices;
                        }
                    }

                    m_deleteDown = true;
                }   
                else
                {
                    m_deleteDown = false;
                }

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

                if (GetCurrentAction() != ActionType_FlipFace)
                {
                    if (glfwGetKey(window, GLFW_KEY_N))
                    {
                        const unsigned int size = m_selectedNodes.size();

                        unsigned int* indices = new unsigned int[size];

                        unsigned int index = 0;
                        for (auto iter = m_selectedNodes.begin(); iter != m_selectedNodes.end(); ++iter)
                        {
                            indices[index++] = *iter;
                        }

                        m_curAction = new FlipFaceAction(m_workspace, indices, size, model);
                        if (!m_workspace->PushAction(m_curAction))
                        {
                            printf("Cannot flip face \n");

                            delete m_curAction;
                            m_curAction = nullptr;
                        }

                        delete[] indices;
                    }
                }
                else if (!glfwGetKey(window, GLFW_KEY_N))
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
                            case ActionType_TranslateObjectRelative:
                            {
                                m_curAction = nullptr;

                                break;
                            }
                            }
                        }


                        const glm::mat4 transformMat = obj->GetGlobalMatrix();

                        switch (m_editorMode)
                        {
                        case EditorMode_Object:
                        {
                            const std::list<Object*> selectedObjects = m_workspace->GetSelectedObjects();

                            glm::vec3 pos = glm::vec3(0);

                            for (auto iter = selectedObjects.begin(); iter != selectedObjects.end(); ++iter)
                            {
                                const glm::mat4 mat = (*iter)->GetGlobalMatrix();

                                pos += mat[3].xyz();
                            }

                            pos /= selectedObjects.size();

                            IsInteractingTransform(pos, glm::vec3(0, 0, 1), curCursorPos, a_winSize, viewProj);
                            IsInteractingTransform(pos, glm::vec3(0, 1, 0), curCursorPos, a_winSize, viewProj);
                            IsInteractingTransform(pos, glm::vec3(1, 0, 0), curCursorPos, a_winSize, viewProj);

                            break;
                        }
                        case EditorMode_Edit:
                        {
                            const Node3Cluster* nodes = model->GetNodes();
                        
                            glm::vec3 pos = glm::vec3(0);

                            for (auto iter = m_selectedNodes.begin(); iter != m_selectedNodes.end(); ++iter)
                            {
                                pos += nodes[*iter].Nodes[0].Node.GetPosition();
                            }

                            pos /= m_selectedNodes.size();

                            const glm::vec4 fPos = transformMat * glm::vec4(pos, 1);
                            if (!IsInteractingCurveNode(fPos.xyz(), glm::vec3(0, 0, 1), curCursorPos, a_winSize, model, viewProj) &&
                                !IsInteractingCurveNode(fPos.xyz(), glm::vec3(0, 1, 0), curCursorPos, a_winSize, model, viewProj) &&
                                !IsInteractingCurveNode(fPos.xyz(), glm::vec3(1, 0, 0), curCursorPos, a_winSize, model, viewProj))
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

                            break;
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
                    const Node3Cluster* nodes = curveModel->GetNodes();

                    switch (GetCurrentAction())
                    {
                    case ActionType_ExtrudeNode:
                    case ActionType_MoveNode:
                    case ActionType_MoveNodeHandle:
                    case ActionType_TranslateObjectRelative:
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
                                if (SelectionControl::NodeInSelection(viewProj, min, max, transformMat, node.Nodes[0].Node))
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
                                if (SelectionControl::NodeInSelection(viewProj, min, max, transformMat, node.Nodes[0].Node))
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
                                if (SelectionControl::NodeInSelection(viewProj, min, max, transformMat, node.Nodes[0].Node))
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
        const glm::vec3 cWorldPos = m_camera->GetScreenToWorld(glm::vec3(curCursorPos, 0.9f), (int)a_winSize.x, (int)a_winSize.y);

        switch (GetCurrentAction())
        {
        case ActionType_ExtrudeNode:
        {
            ExtrudeNodeAction* action = (ExtrudeNodeAction*)m_curAction;
            action->SetPosition(cWorldPos);

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
            action->SetPosition(cWorldPos);

            action->Execute();

            break;
        }
        case ActionType_TranslateObjectRelative:
        {
            TranslateObjectRelativeAction* action = (TranslateObjectRelativeAction*)m_curAction;
            action->SetTranslation(cWorldPos);

            action->Execute();

            break;
        }
        default:
        {
            // Dirty but cannot be stuffed making screen space gizmos
            const glm::vec2 min = glm::min(m_startPos, curCursorPos);
            const glm::vec2 max = glm::max(m_startPos, curCursorPos);

            const glm::vec3 tlWP = m_camera->GetScreenToWorld(glm::vec3(min.x, min.y, -0.99f), (int)a_winSize.x, (int)a_winSize.y);
            const glm::vec3 trWP = m_camera->GetScreenToWorld(glm::vec3(max.x, min.y, -0.99f), (int)a_winSize.x, (int)a_winSize.y);
            const glm::vec3 blWP = m_camera->GetScreenToWorld(glm::vec3(min.x, max.y, -0.99f), (int)a_winSize.x, (int)a_winSize.y);
            const glm::vec3 brWP = m_camera->GetScreenToWorld(glm::vec3(max.x, max.y, -0.99f), (int)a_winSize.x, (int)a_winSize.y);

            const glm::vec3 f = viewInv[2].xyz();

            Gizmos::DrawLine(tlWP, trWP, f, 0.0001f, glm::vec4(0.93f, 0.53f, 0.00f, 1.00f));
            Gizmos::DrawLine(trWP, brWP, f, 0.0001f, glm::vec4(0.93f, 0.53f, 0.00f, 1.00f));
            Gizmos::DrawLine(brWP, blWP, f, 0.0001f, glm::vec4(0.93f, 0.53f, 0.00f, 1.00f));
            Gizmos::DrawLine(blWP, tlWP, f, 0.0001f, glm::vec4(0.93f, 0.53f, 0.00f, 1.00f));

            break;
        }
        } 
    }

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

    switch (m_editorMode)
    {
    case EditorMode_Object:
    {
        const std::list<Object*> objects = m_workspace->GetSelectedObjects();

        const unsigned int objectCount = objects.size();
        if (objectCount > 0)
        {
            glm::vec3 pos = glm::vec3(0);

            for (auto iter = objects.begin(); iter != objects.end(); ++iter)
            {
                const glm::mat4 mat = (*iter)->GetGlobalMatrix();

                pos += mat[3].xyz();
            }

            pos /= objectCount;

            Gizmos::DrawLine(pos.xyz(), pos.xyz() + glm::vec3(0.25f, 0, 0), viewInv[2].xyz(), 0.01f, glm::vec4(1, 0, 0, 1));
            Gizmos::DrawLine(pos.xyz(), pos.xyz() + glm::vec3(0, 0.25f, 0), viewInv[2].xyz(), 0.01f, glm::vec4(0, 1, 0, 1));
            Gizmos::DrawLine(pos.xyz(), pos.xyz() + glm::vec3(0, 0, 0.25f), viewInv[2].xyz(), 0.01f, glm::vec4(0, 0, 1, 1));
        }

        break;
    }
    case EditorMode_Edit:
    {
        const Object* obj = m_workspace->GetSelectedObject();
        if (obj != nullptr)
        {
            const CurveModel* model = obj->GetCurveModel();
            const Node3Cluster* nodes = model->GetNodes();

            glm::vec3 pos = glm::vec3(0);

            for (auto iter = m_selectedNodes.begin(); iter != m_selectedNodes.end(); ++iter)
            {
                pos += nodes[*iter].Nodes[0].Node.GetPosition();
            }

            pos /= m_selectedNodes.size();

            const glm::mat4 transformMat = obj->GetGlobalMatrix(); 

            const glm::vec4 fPos = transformMat * glm::vec4(pos, 1);

            Gizmos::DrawLine(fPos.xyz(), fPos.xyz() + glm::vec3(0.25f, 0, 0), viewInv[2].xyz(), 0.01f, glm::vec4(1, 0, 0, 1));
            Gizmos::DrawLine(fPos.xyz(), fPos.xyz() + glm::vec3(0, 0.25f, 0), viewInv[2].xyz(), 0.01f, glm::vec4(0, 1, 0, 1));
            Gizmos::DrawLine(fPos.xyz(), fPos.xyz() + glm::vec3(0, 0, 0.25f), viewInv[2].xyz(), 0.01f, glm::vec4(0, 0, 1, 1));
        }

        break;
    }
    }

    const int width = m_renderTexture->GetWidth();
    const int height = m_renderTexture->GetHeight();

    const unsigned int handle = m_renderTexture->GetHandle();

    glm::vec4 viewCache;
    int fbCache;

    glGetFloatv(GL_VIEWPORT, (float*)&viewCache);
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint*)&fbCache);

    glViewport(0, 0, width, height);

    glBindFramebuffer(GL_FRAMEBUFFER, handle);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    glClearColor(163.0f / 255, 163.0f / 255, 162.0f / 255, 1);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);  
    
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

    const std::list<Object*> objs = m_workspace->GetObjectList();

    for (auto iter = objs.begin(); iter != objs.end(); ++iter)
    {
        Object* obj = *iter;

        DrawObject(obj, a_winSize);
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

    glEnable(GL_CULL_FACE);  
}