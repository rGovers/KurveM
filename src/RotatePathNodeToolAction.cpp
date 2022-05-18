#include "ToolActions/RotatePathNodeToolAction.h"

#include <glm/gtx/quaternion.hpp>

#include "Actions/RotatePathNodeAction.h"
#include "Camera.h"
#include "ColorTheme.h"
#include "EditorControls/Editor.h"
#include "Gizmos.h"
#include "LocalModel.h"
#include "PathModel.h"
#include "SelectionControl.h"
#include "ToolSettings.h"
#include "Transform.h"
#include "TransformVisualizer.h"
#include "Workspace.h"

RotatePathNodeToolAction::RotatePathNodeToolAction(Workspace* a_workspace, Editor* a_editor)
{
    m_workspace = a_workspace;
    m_editor = a_editor;
}
RotatePathNodeToolAction::~RotatePathNodeToolAction()
{

}

bool RotatePathNodeToolAction::Interact(const glm::mat4& a_viewProj, const glm::vec3& a_pos, const Camera* a_camera, const glm::vec2& a_cursorPos, const glm::vec2& a_screenSize, PathModel* a_model)
{
    const glm::vec3 scaleVec3 = glm::vec3(ToolSettings::EditToolScale);
    constexpr glm::mat4 iden = glm::identity<glm::mat4>();
    constexpr glm::vec4 zeroVec4 = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

    const glm::vec3 axis = AxisControl::GetAxis(Axis_Y);
    const glm::vec3 scaledAxis = axis * ToolSettings::EditToolScale;

    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    if (glm::abs(glm::dot(up, axis)) >= 0.95f)
    {
        up = glm::vec3(0.0f, 0.0f, 1.0f);
    }
    const glm::vec3 right = glm::cross(up, axis);
    up = glm::cross(right, axis);

    const glm::mat4 rot = glm::mat4(glm::vec4(right, 0.0f), glm::vec4(axis, 0.0f), glm::vec4(-up, 0.0f), zeroVec4);
    const glm::mat4 mat = a_viewProj * glm::translate(iden, a_pos) * glm::scale(iden, scaleVec3) * rot;

    const LocalModel* handle = TransformVisualizer::GetRotationHandle();

    if (SelectionControl::PointInMesh(mat, handle, a_cursorPos))
    {
        const glm::vec3 cPos = a_camera->GetScreenToWorld(glm::vec3(a_cursorPos, 0.9f), a_screenSize);
        const unsigned int nodeCount = m_editor->GetSelectedNodeCount();
        const unsigned int* indices = m_editor->GetSelectedNodesArray();

        m_editor->PushAction(new RotatePathNodeAction(m_workspace, indices, nodeCount, a_model, cPos, axis, m_editor->GetMirrorMode()), "Error rotating path node");

        delete[] indices;

        return true;
    }

    return false;
}

bool RotatePathNodeToolAction::LeftClicked(Camera* a_camera, const glm::vec2& a_cursorPos, const glm::vec2& a_screenSize)
{
    const Object* object = m_workspace->GetSelectedObject();
    if (object != nullptr)
    {
        const glm::mat4 transMat = object->GetGlobalMatrix();

        PathModel* model = object->GetPathModel();
        if (model != nullptr)
        {
            const PathNodeCluster* nodes = model->GetPathNodes();

            glm::vec3 pos = glm::vec3(0);

            const std::list<unsigned int> selectedNodes = m_editor->GetSelectedNodes();
            for (auto iter = selectedNodes.begin(); iter != selectedNodes.end(); ++iter)
            {
                pos += nodes[*iter].Nodes[0].Node.GetPosition();
            }

            pos /= selectedNodes.size();

            const glm::vec4 fPos = transMat * glm::vec4(pos, 1.0f);

            const glm::mat4 view = a_camera->GetView();
            const glm::mat4 proj = a_camera->GetProjection(a_screenSize);

            const glm::mat4 viewProj = proj * view;

            if (Interact(viewProj, fPos, a_camera, a_cursorPos, a_screenSize, model))
            {
                return true;
            }
        }
    }

    return false;
}
bool RotatePathNodeToolAction::LeftDown(Camera* a_camera, const glm::vec2& a_cursorPos, const glm::vec2& a_screenSize)
{
    const glm::vec3 cWorldPos = a_camera->GetScreenToWorld(glm::vec3(a_cursorPos, 0.9f), a_screenSize);

    if (m_editor->GetCurrentActionType() == ActionType_RotatePathNode)
    {
        RotatePathNodeAction* action = (RotatePathNodeAction*)m_editor->GetCurrentAction();
        action->SetRotation(cWorldPos);

        action->Execute();

        return true;
    }

    return false;
}
bool RotatePathNodeToolAction::LeftReleased(Camera* a_camera, const glm::vec2& a_cursorPos, const glm::vec2& a_screenSize)
{
    if (m_editor->GetCurrentActionType() == ActionType_RotatePathNode)
    {
        m_editor->SetCurrentAction(nullptr);

        return true;
    }

    return false;
}

void RotatePathNodeToolAction::Draw(Camera* a_camera)
{
    constexpr glm::mat4 iden = glm::identity<glm::mat4>();
    const glm::vec3 scale3 = glm::vec3(ToolSettings::EditToolScale);

    const Object* object = m_workspace->GetSelectedObject();
    if (object != nullptr)
    {
        const PathModel* model = object->GetPathModel();
        if (model != nullptr)
        {
            const Transform* camTransform = a_camera->GetTransform();
            const glm::mat4 viewInv = camTransform->ToMatrix();

            const PathNodeCluster* nodes = model->GetPathNodes();

            glm::vec3 pos = glm::vec3(0.0f);

            const std::list<unsigned int> selectedNodes = m_editor->GetSelectedNodes();
            for (auto iter = selectedNodes.begin(); iter != selectedNodes.end(); ++iter)
            {
                pos += nodes[*iter].Nodes[0].Node.GetPosition();
            }

            pos /= selectedNodes.size();

            const glm::mat4 transformMat = object->GetGlobalMatrix();
            const glm::vec4 fPos = transformMat * glm::vec4(pos, 1.0f);

            const LocalModel* model = TransformVisualizer::GetRotationHandle();

            Gizmos::DrawModel(glm::translate(iden, fPos.xyz()) * glm::scale(iden, scale3), model, ColorTheme::Active);
        }
    }
}