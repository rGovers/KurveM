#include "Editors/EditEditor.h"

#include <glm/gtx/quaternion.hpp>

#include "Actions/FlipFaceAction.h"
#include "Actions/InsertFaceAction.h"
#include "Actions/InsertPathLineAction.h"
#include "Actions/MoveCurveNodeHandleAction.h"
#include "Actions/MovePathNodeHandleAction.h"
#include "Camera.h"
#include "ColorTheme.h"
#include "CurveModel.h"
#include "EditorControls/Editor.h"
#include "Gizmos.h"
#include "Object.h"
#include "PathModel.h"
#include "SelectionControl.h"
#include "ToolActions/ToolAction.h"
#include "ToolActions/ExtrudeArmatureNodeToolAction.h"
#include "ToolActions/ExtrudeCurveNodeToolAction.h"
#include "ToolActions/ExtrudePathNodeToolAction.h"
#include "ToolActions/MoveArmatureNodeToolAction.h"
#include "ToolActions/MoveCurveNodeToolAction.h"
#include "ToolActions/MovePathNodeToolAction.h"
#include "ToolActions/RotateArmatureNodeToolAction.h"
#include "ToolActions/RotateCurveNodeToolAction.h"
#include "ToolActions/RotatePathNodeToolAction.h"
#include "ToolActions/ScaleCurveNodeToolAction.h"
#include "ToolActions/ScalePathNodeToolAction.h"
#include "ToolSettings.h"
#include "Transform.h"
#include "TransformVisualizer.h"
#include "Workspace.h"

EditEditor::EditEditor(Editor* a_editor, Workspace* a_workspace)
{
    m_editor = a_editor;
    m_workspace = a_workspace;

    m_armatureAction = new ToolAction*[ToolMode_End];
    m_curveAction = new ToolAction*[ToolMode_End];
    m_pathAction = new ToolAction*[ToolMode_End];

    for (unsigned int i = 0; i < ToolMode_End; ++i)
    {
        m_armatureAction[i] = nullptr;
        m_curveAction[i] = nullptr;
        m_pathAction[i] = nullptr;
    }

    m_armatureAction[ToolMode_Translate] = new MoveArmatureNodeToolAction(m_workspace, m_editor);
    m_armatureAction[ToolMode_Rotate] = new RotateArmatureNodeToolAction(m_workspace, m_editor);
    m_armatureAction[ToolMode_Extrude] = new ExtrudeArmatureNodeToolAction(m_workspace, m_editor);

    m_curveAction[ToolMode_Translate] = new MoveCurveNodeToolAction(m_workspace, m_editor);
    m_curveAction[ToolMode_Rotate] = new RotateCurveNodeToolAction(m_workspace, m_editor);
    m_curveAction[ToolMode_Scale] = new ScaleCurveNodeToolAction(m_workspace, m_editor);
    m_curveAction[ToolMode_Extrude] = new ExtrudeCurveNodeToolAction(m_workspace, m_editor);

    m_pathAction[ToolMode_Translate] = new MovePathNodeToolAction(m_workspace, m_editor);
    m_pathAction[ToolMode_Rotate] = new RotatePathNodeToolAction(m_workspace, m_editor);
    m_pathAction[ToolMode_Scale] = new ScalePathNodeToolAction(m_workspace, m_editor);
    m_pathAction[ToolMode_Extrude] = new ExtrudePathNodeToolAction(m_workspace, m_editor);
}
EditEditor::~EditEditor()
{
    for (unsigned int i = 0; i < ToolMode_End; ++i)
    {
        if (m_armatureAction[i] != nullptr)
        {
            delete m_armatureAction[i];
        }
        if (m_curveAction[i] != nullptr)
        {
            delete m_curveAction[i];
        }
        if (m_pathAction[i] != nullptr)
        {
            delete m_pathAction[i];
        }
    }

    delete[] m_armatureAction;
    delete[] m_curveAction;
    delete[] m_pathAction;
}

e_EditorMode EditEditor::GetEditorMode()
{
    return EditorMode_Edit;
}

bool EditEditor::IsInteractingCurveNodeHandle(const CurveNodeCluster& a_node, unsigned int a_nodeIndex, CurveModel* a_model, const glm::mat4& a_viewProj, const glm::vec2& a_cursorPos, const glm::mat4& a_transform, const glm::vec3& a_up, const glm::vec3& a_right)
{
    for (auto nodeIter = a_node.Nodes.begin(); nodeIter != a_node.Nodes.end(); ++nodeIter)
    {
        if (SelectionControl::NodeHandleInPoint(a_viewProj, a_cursorPos, ToolSettings::EditHandleSize * 2, a_transform, nodeIter->Node))
        {
            m_editor->PushAction(new MoveCurveNodeHandleAction(m_workspace, nodeIter - a_node.Nodes.begin(), a_nodeIndex, a_model, a_cursorPos, a_right, a_up, m_editor->GetMirrorMode()), "Error moving node handle");

            return true;
        }
    }

    return false;
}
bool EditEditor::IsInteractingPathNodeHandle(const PathNodeCluster& a_node, unsigned int a_nodeIndex, PathModel* a_model, const glm::mat4& a_viewProj, const glm::vec2& a_cursorPos, const glm::mat4& a_transform, const glm::vec3& a_up, const glm::vec3& a_right)
{
    for (auto iter = a_node.Nodes.begin(); iter != a_node.Nodes.end(); ++iter)
    {
        if (SelectionControl::NodeHandleInPoint(a_viewProj, a_cursorPos, ToolSettings::EditHandleSize * 2, a_transform, iter->Node))
        {
            m_editor->PushAction(new MovePathNodeHandleAction(m_workspace, a_nodeIndex, iter - a_node.Nodes.begin(), a_model, a_cursorPos, a_right, a_up, m_editor->GetMirrorMode()), "Error moving node handle");

            return true;
        }
    }

    return false;
}

bool EditEditor::CanInsertCurveFace() const
{
    const Object* obj = m_workspace->GetSelectedObject();
    if (obj != nullptr)
    {
        const CurveModel* curveModel = obj->GetCurveModel();
        if (curveModel != nullptr)
        {
            const unsigned int nodeCount = m_editor->GetSelectedNodeCount();

            return nodeCount == 3 || nodeCount == 4;
        }
    }

    return false;
}
bool EditEditor::IsCurveFaceSelected() const
{
    const Object* obj = m_workspace->GetSelectedObject();
    if (obj != nullptr)
    {
        const CurveModel* curveModel = obj->GetCurveModel();
        if (curveModel != nullptr)
        {
            const unsigned int nodeCount = m_editor->GetSelectedNodeCount();

            const unsigned int* nodes = m_editor->GetSelectedNodesArray();

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

bool EditEditor::CanInsertPathLine() const
{
    const Object* obj = m_workspace->GetSelectedObject();
    if (obj != nullptr)
    {
        const PathModel* pathModel = obj->GetPathModel();
        if (pathModel != nullptr)
        {
            const unsigned int nodeCount = m_editor->GetSelectedNodeCount();

            return nodeCount == 2;
        }
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
    case ObjectType_ArmatureNode:
    {
        const Object* rootObject = m_workspace->GetSelectedObject();

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

                Gizmos::DrawLine(pos, pPos, camFor, 0.005f, ColorTheme::Active);
            }

            const long long id = a_object->GetID();

            const std::list<long long> armNodes = m_editor->GetSelectedArmatureNodes();
            for (auto iter = armNodes.begin(); iter != armNodes.end(); ++iter)
            {
                if (id == *iter)
                {
                    Gizmos::DrawCircleFilled(pos, camFor, 0.025f, 10, ColorTheme::Active);

                    goto NextArmatureDrawNodeLoop;
                }
            }

            Gizmos::DrawCircleFilled(pos, camFor, 0.025f, 10, ColorTheme::InActive);

NextArmatureDrawNodeLoop:;
        }

        break;
    }
    case ObjectType_CurveModel:
    {
        if (obj == a_object)
        {
            const CurveModel* curveModel = a_object->GetCurveModel();

            const int steps = curveModel->GetSteps();

            const CurveFace* faces = curveModel->GetFaces();
            const unsigned int faceCount = curveModel->GetFaceCount();

            const CurveNodeCluster* nodes = curveModel->GetNodes();
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

                    Gizmos::DrawCurve(steps, modelMatrix, sNodes[FaceIndex_3Point_AB], sNodes[FaceIndex_3Point_BA], ColorTheme::Active);
                    Gizmos::DrawCurve(steps, modelMatrix, sNodes[FaceIndex_3Point_AC], sNodes[FaceIndex_3Point_CA], ColorTheme::Active);
                    Gizmos::DrawCurve(steps, modelMatrix, sNodes[FaceIndex_3Point_BC], sNodes[FaceIndex_3Point_CB], ColorTheme::Active);

                    break;
                }
                case FaceMode_4Point:
                {
                    BezierCurveNode3 sNodes[8];

                    for (int j = 0; j < 8; ++j)
                    {
                        sNodes[j] = nodes[face.Index[j]].Nodes[face.ClusterIndex[j]].Node;
                    }

                    Gizmos::DrawCurve(steps, modelMatrix, sNodes[FaceIndex_4Point_AB], sNodes[FaceIndex_4Point_BA], ColorTheme::Active);
                    Gizmos::DrawCurve(steps, modelMatrix, sNodes[FaceIndex_4Point_BD], sNodes[FaceIndex_4Point_DB], ColorTheme::Active);
                    Gizmos::DrawCurve(steps, modelMatrix, sNodes[FaceIndex_4Point_DC], sNodes[FaceIndex_4Point_CD], ColorTheme::Active);
                    Gizmos::DrawCurve(steps, modelMatrix, sNodes[FaceIndex_4Point_CA], sNodes[FaceIndex_4Point_AC], ColorTheme::Active);
                }
                }
            }

            const std::list<unsigned int> selectedNodes = m_editor->GetSelectedNodes();
            for (unsigned int i = 0; i < nodeCount; ++i)
            {
                const BezierCurveNode3 curve = nodes[i].Nodes[0].Node;
                const glm::vec4 pos = modelMatrix * glm::vec4(curve.GetPosition(), 1);

                for (auto iter = selectedNodes.begin(); iter != selectedNodes.end(); ++iter)
                {
                    if (*iter == i)
                    {
                        const std::vector<CurveNode>& nodeCluster = nodes[i].Nodes;
                        for (auto iter = nodeCluster.begin(); iter != nodeCluster.end(); ++iter)
                        {
                            const glm::vec4 pos = modelMatrix * glm::vec4(iter->Node.GetPosition(), 1);

                            if (iter->Node.GetHandlePosition().x != std::numeric_limits<float>::infinity())
                            {
                                const glm::vec4 handlePos = modelMatrix * glm::vec4(iter->Node.GetHandlePosition(), 1);

                                Gizmos::DrawLine(pos, handlePos, camFor, ToolSettings::EditLineSize, ColorTheme::Active);
                                Gizmos::DrawCircleFilled(handlePos, camFor, ToolSettings::EditHandleSize, 10, ColorTheme::Active);
                            }
                            else
                            {
                                Gizmos::DrawCircleFilled(pos, camFor, ToolSettings::EditHandleSize, 10, ColorTheme::Active);
                            }
                        }

                        goto NextCurveDrawNodeLoop;
                    }
                }

                Gizmos::DrawCircleFilled(pos, camFor, ToolSettings::EditInactiveHandleSize, 10, ColorTheme::InActive);

NextCurveDrawNodeLoop:;
            }
        }

        break;
    }
    case ObjectType_PathModel:
    {
        if (obj == a_object)
        {
            const PathModel* pathModel = a_object->GetPathModel();
            const int steps = pathModel->GetPathSteps();

            const unsigned int lineCount = pathModel->GetPathLineCount();
            const PathLine* lines = pathModel->GetPathLines();

            const unsigned int nodeCount = pathModel->GetPathNodeCount();
            const PathNodeCluster* pathNodes = pathModel->GetPathNodes();

            for (unsigned int i = 0; i < lineCount; ++i)
            {
                const PathNode& nodeA = pathNodes[lines[i].Index[0]].Nodes[lines[i].ClusterIndex[0]];
                const PathNode& nodeB = pathNodes[lines[i].Index[1]].Nodes[lines[i].ClusterIndex[1]];

                Gizmos::DrawCurve(steps, modelMatrix, nodeA.Node, nodeB.Node, ColorTheme::Active);
            }

            const std::list<unsigned int> selectedNodes = m_editor->GetSelectedNodes();
            for (unsigned int i = 0; i < nodeCount; ++i)
            {
                const BezierCurveNode3& curve = pathNodes[i].Nodes[0].Node;
                const glm::vec4 pos = modelMatrix * glm::vec4(curve.GetPosition(), 1.0f);

                for (auto iter = selectedNodes.begin(); iter != selectedNodes.end(); ++iter)
                {
                    if (*iter == i)
                    {
                        const std::vector<PathNode>& nodeCluster = pathNodes[i].Nodes;
                        for (auto nIter = nodeCluster.begin(); nIter != nodeCluster.end(); ++nIter)
                        {
                            const glm::vec3 handlePos = nIter->Node.GetHandlePosition();

                            if (handlePos.x != std::numeric_limits<float>::infinity())
                            {
                                const glm::vec4 hPos = modelMatrix * glm::vec4(handlePos, 1.0f);

                                Gizmos::DrawLine(pos, hPos, camFor, ToolSettings::EditLineSize, ColorTheme::Active);
                                Gizmos::DrawCircleFilled(hPos, camFor, ToolSettings::EditHandleSize, 10, ColorTheme::Active);
                            }
                        }

                        goto NextPathDrawNodeLoop;
                    }
                }

                Gizmos::DrawCircleFilled(pos, camFor, ToolSettings::EditInactiveHandleSize, 10, ColorTheme::InActive);

NextPathDrawNodeLoop:;
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

    const Transform* camTransform = a_camera->GetTransform();

    const glm::quat camQuat = camTransform->Quaternion();
    const glm::mat4 camRotMatrix = glm::toMat4(camQuat);

    const glm::vec3 camForward = camRotMatrix[2];
    const glm::vec3 camUp = camRotMatrix[1];
    const glm::vec3 camRight = camRotMatrix[0];

    const e_ToolMode toolMode = m_workspace->GetToolMode();

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

            if (m_armatureAction[toolMode] != nullptr)
            {
                m_armatureAction[toolMode]->LeftClicked(a_camera, a_cursorPos, a_winSize);
            }      

            break;
        }
        case ObjectType_CurveModel:
        {
            CurveModel* model = obj->GetCurveModel();
            if (model != nullptr)
            {
                if (m_curveAction[toolMode] == nullptr || !m_curveAction[toolMode]->LeftClicked(a_camera, a_cursorPos, a_winSize))
                {
                    const CurveNodeCluster* nodes = model->GetNodes();
                    const glm::mat4 transformMat = obj->GetGlobalMatrix();
                    glm::vec3 pos = glm::vec3(0);

                    const std::list<unsigned int> selectedNodes = m_editor->GetSelectedNodes();
                    for (auto iter = selectedNodes.begin(); iter != selectedNodes.end(); ++iter)
                    {
                        pos += nodes[*iter].Nodes[0].Node.GetPosition();
                    }

                    pos /= selectedNodes.size();

                    const glm::vec4 fPos = transformMat * glm::vec4(pos, 1.0f);

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

            break;
        }
        case ObjectType_PathModel:
        {
            PathModel* model = obj->GetPathModel();
            if (model != nullptr)
            {
                if (m_pathAction[toolMode] == nullptr || !m_pathAction[toolMode]->LeftClicked(a_camera, a_cursorPos, a_winSize))
                {
                    const glm::mat4 transformMat = obj->GetGlobalMatrix();
                    const PathNodeCluster* nodes = model->GetPathNodes();

                    glm::vec3 pos = glm::vec3(0);

                    const std::list<unsigned int> selectedNodes = m_editor->GetSelectedNodes();
                    for (auto iter = selectedNodes.begin(); iter != selectedNodes.end(); ++iter)
                    {
                        pos += nodes[*iter].Nodes[0].Node.GetPosition();
                    }

                    pos /= selectedNodes.size();

                    const glm::vec4 fPos = transformMat * glm::vec4(pos, 1.0f);

                    for (auto iter = selectedNodes.begin(); iter != selectedNodes.end(); ++iter)
                    {
                        const unsigned int nodeIndex = *iter;

                        if (IsInteractingPathNodeHandle(nodes[nodeIndex], nodeIndex, model, viewProj, a_cursorPos, transformMat, camUp, camRight))
                        {
                            break;
                        }
                    }   
                }
            }

            break;
        }
        }
    }
}
void EditEditor::LeftDown(double a_delta, Camera* a_camera, const glm::vec2& a_start, const glm::vec2& a_currentPos, const glm::vec2& a_winSize)
{
    const e_ToolMode toolMode = m_workspace->GetToolMode();
    
    const Object* obj = m_workspace->GetSelectedObject();

    const Transform* camTransform = a_camera->GetTransform();
    const glm::mat4 viewInv = camTransform->ToMatrix();

    bool drawSelection = true;

    switch (obj->GetObjectType())
    {
    case ObjectType_Armature:
    {
        drawSelection = m_armatureAction[toolMode] == nullptr || !m_armatureAction[toolMode]->LeftDown(a_camera, a_currentPos, a_winSize);

        break;
    }
    case ObjectType_CurveModel:
    {
        drawSelection = (m_curveAction[toolMode] == nullptr || !m_curveAction[toolMode]->LeftDown(a_camera, a_currentPos, a_winSize)) && m_editor->GetCurrentActionType() != ActionType_MoveCurveNodeHandle;

        break;
    }
    case ObjectType_PathModel:
    {
        drawSelection = (m_pathAction[toolMode] == nullptr || !m_pathAction[toolMode]->LeftDown(a_camera, a_currentPos, a_winSize)) && m_editor->GetCurrentActionType() != ActionType_MovePathNodeHandle;

        break;
    }
    }

    if (drawSelection)
    {
        const glm::vec2 min = glm::min(a_start, a_currentPos);
        const glm::vec2 max = glm::max(a_start, a_currentPos);

        const glm::vec3 tlWP = a_camera->GetScreenToWorld(glm::vec3(min.x, min.y, -0.99f), (int)a_winSize.x, (int)a_winSize.y);
        const glm::vec3 trWP = a_camera->GetScreenToWorld(glm::vec3(max.x, min.y, -0.99f), (int)a_winSize.x, (int)a_winSize.y);
        const glm::vec3 blWP = a_camera->GetScreenToWorld(glm::vec3(min.x, max.y, -0.99f), (int)a_winSize.x, (int)a_winSize.y);
        const glm::vec3 brWP = a_camera->GetScreenToWorld(glm::vec3(max.x, max.y, -0.99f), (int)a_winSize.x, (int)a_winSize.y);

        const glm::vec3 f = viewInv[2].xyz();

        Gizmos::DrawLine(tlWP, trWP, f, 0.0001f, ColorTheme::Active);
        Gizmos::DrawLine(trWP, brWP, f, 0.0001f, ColorTheme::Active);
        Gizmos::DrawLine(brWP, blWP, f, 0.0001f, ColorTheme::Active);
        Gizmos::DrawLine(blWP, tlWP, f, 0.0001f, ColorTheme::Active);
    }
}
void EditEditor::LeftReleased(Camera* a_camera, const glm::vec2& a_start, const glm::vec2& a_end, const glm::vec2& a_winSize)
{
    const glm::mat4 view = a_camera->GetView();
    const glm::mat4 proj = a_camera->GetProjection((int)a_winSize.x, (int)a_winSize.y);

    const glm::mat4 viewProj = proj * view;

    const glm::vec2 min = glm::min(a_start, a_end);
    const glm::vec2 max = glm::max(a_start, a_end);

    const ImGuiIO io = ImGui::GetIO();

    const e_ToolMode toolMode = m_workspace->GetToolMode();

    const Object* obj = m_workspace->GetSelectedObject();
    if (obj != nullptr)
    {
        const glm::mat4 transformMat = obj->GetGlobalMatrix();

        const e_ObjectType objectType = obj->GetObjectType();
        switch (objectType)
        {
        case ObjectType_Armature:
        {
            if (m_armatureAction[toolMode] == nullptr || !m_armatureAction[toolMode]->LeftReleased(a_camera, a_end, a_winSize))
            {
                const Object* obj = m_workspace->GetSelectedObject();

                const std::list<Object*> children = obj->GetChildren();

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
            }

            break;
        }
        case ObjectType_CurveModel:
        {
            if (m_curveAction[toolMode] == nullptr || !m_curveAction[toolMode]->LeftReleased(a_camera, a_end, a_winSize))
            {
                const CurveModel* model = obj->GetCurveModel();
                const unsigned int nodeCount = model->GetNodeCount();
                const CurveNodeCluster* nodes = model->GetNodes();

                const e_ActionType actionType = m_editor->GetCurrentActionType();

                switch (actionType)
                {
                case ActionType_MoveCurveNodeHandle:
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
                                const CurveNodeCluster& node = nodes[i];
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
                                const CurveNodeCluster& node = nodes[i];
                                if (SelectionControl::NodeInSelection(viewProj, min, max, transformMat, node.Nodes[0].Node))
                                {
                                    const std::list<unsigned int> selectedNodes = m_editor->GetSelectedNodes();
                                    for (auto iter = selectedNodes.begin(); iter != selectedNodes.end(); ++iter)
                                    {
                                        if (*iter == i)
                                        {
                                            m_editor->RemoveNodeFromSelection(i);

                                            goto NextCurveNode;
                                        }
                                    }

                                    m_editor->AddNodeToSelection(i);

NextCurveNode:;
                                }
                            }
                        }
                        else
                        {
                            m_editor->ClearSelectedNodes();

                            for (unsigned int i = 0; i < nodeCount; ++i)
                            {
                                const CurveNodeCluster& node = nodes[i];
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
        case ObjectType_PathModel:
        {
            if (m_pathAction[toolMode] == nullptr || !m_pathAction[toolMode]->LeftReleased(a_camera, a_end, a_winSize))
            {
                const PathModel* model = obj->GetPathModel();
                const unsigned int nodeCount = model->GetPathNodeCount();
                const PathNodeCluster* nodes = model->GetPathNodes();

                switch (m_editor->GetCurrentActionType())
                {
                case ActionType_MovePathNodeHandle:
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
                                const PathNodeCluster& node = nodes[i];
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
                                const PathNodeCluster& node = nodes[i];
                                if (SelectionControl::NodeInSelection(viewProj, min, max, transformMat, node.Nodes[0].Node))
                                {
                                    const std::list<unsigned int> selectedNodes = m_editor->GetSelectedNodes();
                                    for (auto iter = selectedNodes.begin(); iter != selectedNodes.end(); ++iter)
                                    {
                                        if (*iter == i)
                                        {
                                            m_editor->RemoveNodeFromSelection(i);

                                            goto NextPathNode;
                                        }
                                    }

                                    m_editor->AddNodeToSelection(i);

NextPathNode:;
                                }
                            }
                        }
                        else
                        {
                            m_editor->ClearSelectedNodes();

                            for (unsigned int i = 0; i < nodeCount; ++i)
                            {
                                const PathNodeCluster& node = nodes[i];
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
        }
    }
}

void EditEditor::Update(Camera* a_camera, const glm::vec2& a_cursorPos, const glm::vec2& a_screenSize, double a_delta)
{
    constexpr glm::mat4 iden = glm::identity<glm::mat4>();
    constexpr float pi = glm::pi<float>();
    constexpr float halfPi = pi * 0.5f;

    const Transform* camTransform = a_camera->GetTransform();
    const glm::mat4 viewInv = camTransform->ToMatrix();

    const e_ToolMode toolMode = m_workspace->GetToolMode();

    const Object* obj = m_workspace->GetSelectedObject();
    if (obj != nullptr)
    {
        const e_ObjectType objectType = obj->GetObjectType();
        switch (objectType)
        {
        case ObjectType_Armature:
        {
            if (m_armatureAction[toolMode] != nullptr)
            {
                m_armatureAction[toolMode]->Draw(a_camera);
            }

            break;
        }
        case ObjectType_CurveModel:
        {
            if (m_curveAction[toolMode] != nullptr)
            {
                m_curveAction[toolMode]->Draw(a_camera);
            }

            break;
        }
        case ObjectType_PathModel:
        {
            if (m_pathAction[toolMode] != nullptr)
            {
                m_pathAction[toolMode]->Draw(a_camera);
            }

            break;
        }
        }
    }
}
void EditEditor::UpdateContextMenu(const glm::vec2& a_winPos, const glm::vec2& a_winSize)
{
    const Object* obj = m_workspace->GetSelectedObject();
    if (obj != nullptr)
    {
        const unsigned int nodeCount = m_editor->GetSelectedNodeCount();
        const unsigned int* nodes = m_editor->GetSelectedNodesArray();

        CurveModel* curveModel = obj->GetCurveModel();
        if (curveModel != nullptr)
        {
            if (ImGui::MenuItem("Insert Face", nullptr, false, CanInsertCurveFace()))
            {
                Action* action = new InsertFaceAction(m_workspace, nodes, nodeCount, curveModel);
                if (!m_workspace->PushAction(action))
                {
                    printf("Error creating face \n");

                    delete action;
                }
            }

            if (ImGui::MenuItem("Flip Face", nullptr, false, IsCurveFaceSelected()))
            {
                Action* action = new FlipFaceAction(m_workspace, nodes, nodeCount, curveModel);
                if (!m_workspace->PushAction(action))
                {
                    printf("Error fliping face \n");

                    delete action;
                }
            }
        }

        PathModel* pathModel = obj->GetPathModel();
        if (pathModel != nullptr)
        {
            if (ImGui::MenuItem("Insert Line", nullptr, false, CanInsertPathLine()))
            {
                Action* action = new InsertPathLineAction(m_workspace, nodes, nodeCount, pathModel);
                if (!m_workspace->PushAction(action))
                {
                    printf("Error inserting path line \n");

                    delete action;
                }
            }
        }

        delete[] nodes;
    }
}