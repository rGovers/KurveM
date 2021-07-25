#include "Editors/EditEditor.h"

#include <glm/gtx/quaternion.hpp>

#include "Actions/ExtrudeArmatureNodeAction.h"
#include "Actions/ExtrudeNodeAction.h"
#include "Actions/MoveNodeAction.h"
#include "Actions/MoveNodeHandleAction.h"
#include "Actions/TranslateObjectRelativeAction.h"
#include "Camera.h"
#include "ColorTheme.h"
#include "Editor.h"
#include "Gizmos.h"
#include "Object.h"
#include "SelectionControl.h"
#include "Transform.h"
#include "Workspace.h"

EditEditor::EditEditor(Editor* a_editor, Workspace* a_workspace)
{
    m_editor = a_editor;

    m_workspace = a_workspace;
}
EditEditor::~EditEditor()
{

}

e_EditorMode EditEditor::GetEditorMode()
{
    return EditorMode_Edit;
}

bool EditEditor::IsInteractingCurveNode(Camera* a_camera, const glm::vec3& a_pos, const glm::vec3& a_axis, const glm::vec2& a_cursorPos, const glm::vec2& a_screenSize, CurveModel* a_model, const glm::mat4& a_viewProj)
{
    if (SelectionControl::PointInPoint(a_viewProj, a_cursorPos, 0.025f, a_pos + a_axis * 0.3f))
    {
        const glm::vec3 cPos = a_camera->GetScreenToWorld(glm::vec3(a_cursorPos, 0.9f), (int)a_screenSize.x, (int)a_screenSize.y);

        const unsigned int nodeCount = m_editor->GetSelectedNodeCount();
        unsigned int* indices = m_editor->GetSelectedNodesArray();

        switch (m_workspace->GetToolMode())
        {
        case ToolMode_Translate:
        {
            Action* action = new MoveNodeAction(m_workspace, indices, nodeCount, a_model, cPos, a_axis);
            if (!m_workspace->PushAction(action))
            {
                printf("Error moving node \n");

                delete action;
            }
            else
            {
                m_editor->SetCurrentAction(action);
            }

            break;
        }
        case ToolMode_Extrude:
        {
            Action* action = new ExtrudeNodeAction(m_workspace, m_editor, indices, nodeCount, a_model, cPos, a_axis);
            if (!m_workspace->PushAction(action))
            {
                printf("Error extruding node \n");

                delete action;
            }
            else
            {
                m_editor->SetCurrentAction(action);
            }

            break;
        }
        }
        
        delete[] indices;

        return true;
    }

    return false;
}
bool EditEditor::IsInteractingCurveNodeHandle(const Node3Cluster& a_node, unsigned int a_nodeIndex, CurveModel* a_model, const glm::mat4& a_viewProj, const glm::vec2& a_cursorPos, const glm::mat4& a_transform, const glm::vec3& a_up, const glm::vec3& a_right)
{
    for (auto nodeIter = a_node.Nodes.begin(); nodeIter != a_node.Nodes.end(); ++nodeIter)
    {
        if (SelectionControl::NodeHandleInPoint(a_viewProj, a_cursorPos, 0.025f, a_transform, nodeIter->Node))
        {
            Action* action = new MoveNodeHandleAction(m_workspace, nodeIter - a_node.Nodes.begin(), a_nodeIndex, a_model, a_cursorPos, a_right, a_up);
            if (!m_workspace->PushAction(action))
            {
                printf("Error moving node handle \n");

                delete action;
            }
            else
            {
                m_editor->SetCurrentAction(action);
            }

            return true;
        }
    }

    return false;
}

bool EditEditor::InteractingArmatureNode(Camera* a_camera, const glm::vec3& a_pos, const glm::vec3& a_axis, const glm::vec2& a_cursorPos, const glm::vec2& a_screenSize, const glm::mat4& a_viewProj)
{
    if (SelectionControl::PointInPoint(a_viewProj, a_cursorPos, 0.025f, a_pos + a_axis * 0.3f))
    {
        const glm::vec3 cPos = a_camera->GetScreenToWorld(glm::vec3(a_cursorPos, 0.9f), (int)a_screenSize.x, (int)a_screenSize.y);

        const int objCount = m_editor->GetSelectedArmatureNodesCount();
        Object** objs = m_editor->GetSelectedArmatureObjectsArray();

        const e_ToolMode toolMode = m_workspace->GetToolMode();
        switch (toolMode)
        {
        case ToolMode_Translate:
        {
            Action *action = new TranslateObjectRelativeAction(cPos, a_axis, objs, objCount);
            if (!m_workspace->PushAction(action))
            {
                printf("Error moving armature node \n");

                delete action;
            }
            else
            {
                m_editor->SetCurrentAction(action);
            }

            break;
        }
        case ToolMode_Extrude:
        {
            Action* action = new ExtrudeArmatureNodeAction(m_editor, objs, objCount, cPos, a_axis);
            if (!m_workspace->PushAction(action))
            {
                printf("Error extruding armature node \n");

                delete action;
            }
            else
            {
                m_editor->SetCurrentAction(action);
            }

            break;
        }
        }

        delete[] objs;

        return true;
    }

    return false;
}

bool EditEditor::SelectArmatureNodes(Object* a_node, const glm::mat4& a_viewProj, const glm::vec2& a_start, const glm::vec2& a_end)
{
    bool ret = false;

    if (SelectionControl::ObjectPointInSelection(a_node, a_viewProj, a_start, a_end))
    {
        const ImGuiIO io = ImGui::GetIO();

        if (ImGui::IsWindowFocused())
        {
            if (io.KeyShift)
            {
                m_editor->AddArmatureNodeToSelection(a_node);
            }
            else if (io.KeyCtrl)
            {
                const long long id = a_node->GetID();

                const std::list<long long> selectedArmatures = m_editor->GetSelectedArmatureNodes();

                bool found = false;
                for (auto iter = selectedArmatures.begin(); iter != selectedArmatures.end(); ++iter)
                {
                    if (*iter == id)
                    {
                        found = true;

                        m_editor->RemoveArmatureNodeFromSelection(a_node);

                        break;
                    } 
                }

                if (!found)
                {
                    m_editor->AddArmatureNodeToSelection(a_node);
                }
            }
            else
            {
                m_editor->AddArmatureNodeToSelection(a_node);
            }

            ret = true;
        }
    }

    const std::list<Object*> children = a_node->GetChildren();
    for (auto iter = children.begin(); iter != children.end(); ++iter)
    {
        const e_ObjectType objectType = (*iter)->GetObjectType();

        if (objectType == ObjectType_ArmatureNode)
        {
            SelectArmatureNodes(*iter, a_viewProj, a_start, a_end);
        }
    }

    return ret;
}

void DrawCurve(int a_steps, const glm::mat4& a_modelMatrix, BezierCurveNode3& a_nodeA, BezierCurveNode3& a_nodeB)
{
    for (int i = 0; i < a_steps; ++i)
    {
        const glm::vec3 pointA = (a_modelMatrix * glm::vec4(BezierCurveNode3::GetPoint(a_nodeA, a_nodeB, (float)i / a_steps), 1)).xyz();
        const glm::vec3 pointB = (a_modelMatrix * glm::vec4(BezierCurveNode3::GetPoint(a_nodeA, a_nodeB, (float)(i + 1) / a_steps), 1)).xyz();

        Gizmos::DrawLine(pointA, pointB, 0.0025f, ColorTheme::Active);
    }
}

void EditEditor::DrawObject(Camera* a_camera, Object* a_object, const glm::vec2& a_winSize)
{
    const Transform* camTransform = a_camera->GetTransform();

    const glm::mat4 camTransformMatrix = camTransform->ToMatrix();
    const glm::mat4 modelMatrix = a_object->GetGlobalMatrix();

    const glm::vec3 camFor = glm::normalize(camTransformMatrix[2]);

    a_object->DrawBase(a_camera, a_winSize);

    const Object* obj = m_workspace->GetSelectedObject();

    switch (a_object->GetObjectType())
    {
    case ObjectType_CurveModel:
    {
        if (obj == a_object)
        {
            const CurveModel *curveModel = a_object->GetCurveModel();

            const int steps = curveModel->GetSteps();

            const CurveFace *faces = curveModel->GetFaces();
            const unsigned int faceCount = curveModel->GetFaceCount();

            const Node3Cluster *nodes = curveModel->GetNodes();
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
                const std::list<unsigned int> selectedNodes = m_editor->GetSelectedNodes();
                for (auto iter = selectedNodes.begin(); iter != selectedNodes.end(); ++iter)
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

                    Gizmos::DrawCircleFilled(pos, camFor, 0.025f, 10, ColorTheme::InActive);
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

                            Gizmos::DrawLine(pos, handlePos, camFor, 0.005f, ColorTheme::Active);
                            Gizmos::DrawCircleFilled(handlePos, camFor, 0.05f, 15, ColorTheme::Active);
                        }
                        else
                        {
                            Gizmos::DrawCircleFilled(pos, camFor, 0.05f, 15, ColorTheme::Active);
                        }
                    }
                }
            }
        }

        break;
    }
    case ObjectType_ArmatureNode:
    {
        const Object *rootObject = m_workspace->GetSelectedObject();

        if (a_object->GetRootObject() == rootObject)
        {
            const glm::vec3 pos = modelMatrix[3];

            const Object* parent = a_object->GetParent();
            const e_ObjectType pType = parent->GetObjectType();
            if (pType == ObjectType_ArmatureNode)
            {
                const glm::vec3 pPos = parent->GetGlobalTranslation();

                const glm::vec3 diff = pos - pPos;

                const float len = glm::length(diff);

                // Gizmos::DrawTriangle(pPos + diff * 0.5f, diff / len, camFor, 0.1f, glm::vec4(0.93f, 0.53f, 0.00f, 1.00f));

                Gizmos::DrawLine(pos, pPos, camFor, 0.005f, ColorTheme::Active);
            }

            bool selected = false;

            const long long id = a_object->GetID();

            const std::list<long long> armNodes = m_editor->GetSelectedArmatureNodes();
            for (auto iter = armNodes.begin(); iter != armNodes.end(); ++iter)
            {
                if (id == *iter)
                {
                    selected = true;

                    break;
                }
            }

            if (!selected)
            {
                Gizmos::DrawCircleFilled(pos, camFor, 0.025f, 10, ColorTheme::InActive);
            }
            else
            {
                Gizmos::DrawCircleFilled(pos, camFor, 0.025f, 10, ColorTheme::Active);
            }
        }

        break;
    }
    }
}

void EditEditor::LeftClicked(Camera* a_camera, const glm::vec2& a_cursorPos, const glm::vec2& a_winSize)
{
    const glm::mat4 view = a_camera->GetView();
    const glm::mat4 proj = a_camera->GetProjection((int)a_winSize.x, (int)a_winSize.y);

    const glm::mat4 viewProj = proj * view;

    Transform* camTransform = a_camera->GetTransform();

    const glm::quat camQuat = camTransform->Quaternion();
    const glm::mat4 camRotMatrix = glm::toMat4(camQuat);

    const glm::vec3 camForward = camRotMatrix[2];
    const glm::vec3 camUp = camRotMatrix[1];
    const glm::vec3 camRight = camRotMatrix[0];

    const Object* obj = m_workspace->GetSelectedObject();
    if (obj != nullptr)
    {
        const e_ObjectType objectType = obj->GetObjectType();
        switch (objectType)
        {
        case ObjectType_Armature:
        {
            const std::list<long long> selectedNodes = m_editor->GetSelectedArmatureNodes();

            glm::vec3 pos = glm::vec3(0);

            int nodeCount = 0;
            for (auto iter = selectedNodes.begin(); iter != selectedNodes.end(); ++iter)
            {
                const Object* obj = m_workspace->GetObject(*iter);

                if (obj != nullptr)
                {
                    const glm::mat4 mat = obj->GetGlobalMatrix();

                    pos += mat[3].xyz();
                    ++nodeCount;
                }
            }

            pos /= nodeCount;

            if (!InteractingArmatureNode(a_camera, pos, glm::vec3(0, 0, 1), a_cursorPos, a_winSize, viewProj))
            {
                if (!InteractingArmatureNode(a_camera, pos, glm::vec3(0, 1, 0), a_cursorPos, a_winSize, viewProj))
                {
                    InteractingArmatureNode(a_camera, pos, glm::vec3(1, 0, 0), a_cursorPos, a_winSize, viewProj);
                }
            }            

            break;
        }
        case ObjectType_CurveModel:
        {
            const glm::mat4 transformMat = obj->GetGlobalMatrix();

            CurveModel* model = obj->GetCurveModel();
            if (model != nullptr)
            {
                const Node3Cluster* nodes = model->GetNodes();

                glm::vec3 pos = glm::vec3(0);

                const std::list<unsigned int> selectedNodes = m_editor->GetSelectedNodes();
                for (auto iter = selectedNodes.begin(); iter != selectedNodes.end(); ++iter)
                {
                    pos += nodes[*iter].Nodes[0].Node.GetPosition();
                }

                pos /= selectedNodes.size();

                const glm::vec4 fPos = transformMat * glm::vec4(pos, 1);
                if (!IsInteractingCurveNode(a_camera, fPos, glm::vec3(0, 0, 1), a_cursorPos, a_winSize, model, viewProj) &&
                    !IsInteractingCurveNode(a_camera, fPos, glm::vec3(0, 1, 0), a_cursorPos, a_winSize, model, viewProj) &&
                    !IsInteractingCurveNode(a_camera, fPos, glm::vec3(1, 0, 0), a_cursorPos, a_winSize, model, viewProj))
                {
                    for (auto iter = selectedNodes.begin(); iter != selectedNodes.end(); ++iter)
                    {
                        const unsigned int nodeIndex = *iter;

                        if (IsInteractingCurveNodeHandle(nodes[nodeIndex], nodeIndex, model, viewProj, a_cursorPos, transformMat, camUp, camRight))
                        {
                            break;
                        }
                    }
                }
            }
        }
        }
    }
}
void EditEditor::LeftDown(double a_delta, Camera* a_camera, const glm::vec2& a_start, const glm::vec2& a_currentPos, const glm::vec2& a_winSize)
{

}
void EditEditor::LeftReleased(Camera* a_camera, const glm::vec2& a_start, const glm::vec2& a_end, const glm::vec2& a_winSize)
{
    const glm::mat4 view = a_camera->GetView();
    const glm::mat4 proj = a_camera->GetProjection((int)a_winSize.x, (int)a_winSize.y);

    const glm::mat4 viewProj = proj * view;

    const glm::vec2 min = glm::min(a_start, a_end);
    const glm::vec2 max = glm::max(a_start, a_end);

    const ImGuiIO io = ImGui::GetIO();

    const Object* obj = m_workspace->GetSelectedObject();
    if (obj != nullptr)
    {
        const glm::mat4 transformMat = obj->GetGlobalMatrix();

        const e_ObjectType objectType = obj->GetObjectType();
        switch (objectType)
        {
        case ObjectType_CurveModel:
        {
            const CurveModel *curveModel = obj->GetCurveModel();
            if (curveModel != nullptr)
            {
                const unsigned int nodeCount = curveModel->GetNodeCount();
                const Node3Cluster *nodes = curveModel->GetNodes();

                const e_ActionType actionType = m_editor->GetCurrentActionType();

                switch (actionType)
                {
                case ActionType_ExtrudeNode:
                case ActionType_MoveNode:
                case ActionType_MoveNodeHandle:
                {
                    m_editor->SetCurrentAction(nullptr);

                    break;
                }
                default:
                {
                    if (ImGui::IsWindowFocused())
                    {
                        if (io.KeyShift)
                        {
                            for (unsigned int i = 0; i < nodeCount; ++i)
                            {
                                const Node3Cluster node = nodes[i];
                                if (SelectionControl::NodeInSelection(viewProj, min, max, transformMat, node.Nodes[0].Node))
                                {
                                    m_editor->AddNodeToSelection(i);
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

                                    const std::list<unsigned int> selectedNodes = m_editor->GetSelectedNodes();
                                    for (auto iter = selectedNodes.begin(); iter != selectedNodes.end(); ++iter)
                                    {
                                        if (*iter == i)
                                        {
                                            found = true;

                                            m_editor->RemoveNodeFromSelection(i);

                                            break;
                                        }
                                    }

                                    if (!found)
                                    {
                                        m_editor->AddNodeToSelection(i);
                                    }
                                }
                            }
                        }
                        else
                        {
                            m_editor->ClearSelectedNodes();

                            for (unsigned int i = 0; i < nodeCount; ++i)
                            {
                                const Node3Cluster node = nodes[i];
                                if (SelectionControl::NodeInSelection(viewProj, min, max, transformMat, node.Nodes[0].Node))
                                {
                                    m_editor->AddNodeToSelection(i);
                                }
                            }
                        }
                    }

                    break;
                }
                }
            }

            break;
        }
        case ObjectType_Armature:
        {
            const e_ActionType actionType = m_editor->GetCurrentActionType();
            switch (actionType)
            {
            case ActionType_TranslateObjectRelative:
            {
                m_editor->SetCurrentAction(nullptr);

                break;
            }
            default:
            {
                const Object *obj = m_workspace->GetSelectedObject();

                const std::list<Object *> children = obj->GetChildren();

                if (!io.KeyCtrl && !io.KeyShift)
                {
                    m_editor->ClearSelectedNodes();
                }

                for (auto iter = children.begin(); iter != children.end(); ++iter)
                {
                    const e_ObjectType objType = (*iter)->GetObjectType();

                    if (objType == ObjectType_ArmatureNode)
                    {
                        SelectArmatureNodes(*iter, viewProj, min, max);
                    }
                }

                break;
            }

            break;
            }
        }
        }
    }
}

void EditEditor::Update(Camera* a_camera, const glm::vec2& a_cursorPos, const glm::vec2& a_screenSize, double a_delta)
{
    Transform* camTransform = a_camera->GetTransform();
    const glm::mat4 viewInv = camTransform->ToMatrix();

    Object* obj = m_workspace->GetSelectedObject();
    if (obj != nullptr)
    {
        const e_ObjectType objectType = obj->GetObjectType();
        switch (objectType)
        {
        case ObjectType_CurveModel:
        {
            const CurveModel* model = obj->GetCurveModel();
            if (model != nullptr)
            {
                const Node3Cluster* nodes = model->GetNodes();

                glm::vec3 pos = glm::vec3(0);

                const std::list<unsigned int> selectedNodes = m_editor->GetSelectedNodes();
                for (auto iter = selectedNodes.begin(); iter != selectedNodes.end(); ++iter)
                {
                    pos += nodes[*iter].Nodes[0].Node.GetPosition();
                }

                pos /= selectedNodes.size();

                const glm::mat4 transformMat = obj->GetGlobalMatrix();

                const glm::vec4 fPos = transformMat * glm::vec4(pos, 1);

                const e_ToolMode toolMode = m_workspace->GetToolMode();
                switch (toolMode)
                {
                case ToolMode_Scale:
                {
                    Gizmos::DrawScale(fPos, viewInv[2], 0.25f);

                    break;
                }
                default:
                {   
                    Gizmos::DrawTranslation(fPos, viewInv[2], 0.25f);

                    break;
                }
                }
            }

            break;
        }
        case ObjectType_Armature:
        {
            glm::vec3 pos = glm::vec3(0);

            int nodeCount = 0;

            const std::list<long long> selectedNodes = m_editor->GetSelectedArmatureNodes();

            for (auto iter = selectedNodes.begin(); iter != selectedNodes.end(); ++iter)
            {
                const Object* obj = m_workspace->GetObject(*iter);

                if (obj != nullptr)
                {
                    glm::mat4 mat = obj->GetGlobalMatrix();

                    pos += mat[3].xyz();
                    ++nodeCount;
                }
            }

            pos /= nodeCount;

            const e_ToolMode toolMode = m_workspace->GetToolMode();
            switch (toolMode)
            {
            case ToolMode_Scale:
            {
                Gizmos::DrawScale(pos, viewInv[2], 0.25f);

                break;
            }
            default:
            {
                Gizmos::DrawTranslation(pos, viewInv[2], 0.25f);

                break;
            }
            }
        }
        }
    }
}