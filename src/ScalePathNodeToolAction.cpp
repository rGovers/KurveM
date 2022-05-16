#include "ToolActions/ScalePathNodeToolAction.h"

#include <glm/gtx/quaternion.hpp>

#include "Actions/ScalePathNodeAction.h"
#include "Camera.h"
#include "EditorControls/Editor.h"
#include "Gizmos.h"
#include "LocalModel.h"
#include "PathModel.h"
#include "SelectionControl.h"
#include "Transform.h"
#include "TransformVisualizer.h"
#include "Workspace.h"

ScalePathNodeToolAction::ScalePathNodeToolAction(Workspace* a_workspace, Editor* a_editor)
{
    m_workspace = a_workspace;
    m_editor = a_editor;
}
ScalePathNodeToolAction::~ScalePathNodeToolAction()
{

}

bool ScalePathNodeToolAction::Interact(const glm::mat4& a_viewProj, const glm::vec3& a_pos, e_Axis a_axis, const Camera* a_camera, const glm::vec2& a_cursorPos, const glm::vec2& a_screenSize, PathModel* a_model)
{
    constexpr float scale = 0.25f;
    constexpr glm::vec3 scaleVec3 = glm::vec3(scale);
    constexpr glm::mat4 iden = glm::identity<glm::mat4>();
    constexpr glm::vec4 zeroVec4 = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

    const glm::vec3 axis = AxisControl::GetAxis(a_axis);
    const glm::vec3 scaledAxis = axis * scale;

    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    if (glm::abs(glm::dot(up, axis)) >= 0.95f)
    {
        up = glm::vec3(0.0f, 0.0f, 1.0f);
    }
    const glm::vec3 right = glm::cross(up, axis);
    up = glm::cross(right, axis);

    const glm::mat4 rot = glm::mat4(glm::vec4(right, 0.0f), glm::vec4(up, 0.0f), glm::vec4(axis, 0.0f), zeroVec4);
    const glm::mat4 mat = a_viewProj * glm::translate(iden, a_pos + scaledAxis) * glm::scale(iden, scaleVec3) * rot;

    const LocalModel* handle = TransformVisualizer::GetScaleHandle();

    if (SelectionControl::PointInMesh(mat, handle, a_cursorPos))
    {
        const glm::vec3 cPos = a_camera->GetScreenToWorld(glm::vec3(a_cursorPos, 0.9f), a_screenSize);
        const unsigned int nodeCount = m_editor->GetSelectedNodeCount();
        const unsigned int* indices = m_editor->GetSelectedNodesArray();

        m_editor->PushAction(new ScalePathNodeAction(m_workspace, indices, nodeCount, a_model, cPos, axis, m_editor->GetMirrorMode()), "Error scaling path node");

        delete[] indices;

        return true;
    }
    
    return false;
}

bool ScalePathNodeToolAction::LeftClicked(Camera* a_camera, const glm::vec2& a_cursorPos, const glm::vec2& a_winSize)
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
            const glm::mat4 proj = a_camera->GetProjection(a_winSize);

            const glm::mat4 viewProj = proj * view;

            if (Interact(viewProj, fPos, Axis_X, a_camera, a_cursorPos, a_winSize, model) || 
                Interact(viewProj, fPos, Axis_Z, a_camera, a_cursorPos, a_winSize, model))
            {
                return true;
            }
        }
    }

    return false;
}
bool ScalePathNodeToolAction::LeftDown(Camera* a_camera, const glm::vec2& a_cursorPos, const glm::vec2& a_screenSize)
{
    const glm::vec3 cWorldPos = a_camera->GetScreenToWorld(glm::vec3(a_cursorPos, 0.9f), a_screenSize);

    if (m_editor->GetCurrentActionType() == ActionType_ScalePathNode)
    {
        ScalePathNodeAction* action = (ScalePathNodeAction*)m_editor->GetCurrentAction();
        action->SetScale(cWorldPos);

        action->Execute();

        return true;
    }

    return false;
}
bool ScalePathNodeToolAction::LeftReleased(Camera* a_camera, const glm::vec2& a_cursorPos, const glm::vec2& a_screenSize)
{
    if (m_editor->GetCurrentActionType() == ActionType_ScalePathNode)
    {
        m_editor->SetCurrentAction(nullptr);

        return true;
    }

    return false;
}

void ScalePathNodeToolAction::Draw(Camera* a_camera)
{
    constexpr glm::mat4 iden = glm::identity<glm::mat4>();
    constexpr float pi = glm::pi<float>();
    constexpr float halfPi = pi * 0.5f;
    constexpr float scale = 0.25f;
    constexpr glm::vec3 scale3 = glm::vec3(scale);

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

            const LocalModel* model = TransformVisualizer::GetScaleHandle();

            const glm::vec3 xAxis = glm::vec3(scale, 0.0f, 0.0f);
            const glm::vec3 yAxis = glm::vec3(0.0f, 0.0f, scale);

            const glm::mat4 transform = glm::translate(iden, fPos.xyz()) * glm::scale(iden, scale3);

            const glm::mat4 rightTransform = transform * glm::toMat4(glm::angleAxis(-halfPi, glm::vec3(0.0f, 0.0f, 1.0f)));
            const glm::mat4 forwardTransform = transform * glm::toMat4(glm::angleAxis(halfPi, glm::vec3(1.0f, 0.0f, 0.0f)));

            Gizmos::DrawModel(glm::translate(iden, xAxis) * rightTransform, model, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
            Gizmos::DrawModel(glm::translate(iden, yAxis) * forwardTransform, model, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

            Gizmos::DrawLine(fPos, fPos.xyz() + xAxis, viewInv[2], 0.01f, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
            Gizmos::DrawLine(fPos, fPos.xyz() + yAxis, viewInv[2], 0.01f, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
        }
    }
}