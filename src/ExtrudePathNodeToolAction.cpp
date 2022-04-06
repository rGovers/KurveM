#include "ToolActions/ExtrudePathNodeToolAction.h"

#include <glm/gtx/quaternion.hpp>

#include "Actions/ExtrudePathNodeAction.h"
#include "Camera.h"
#include "EditorControls/Editor.h"
#include "Gizmos.h"
#include "PathModel.h"
#include "SelectionControl.h"
#include "Transform.h"
#include "TransformVisualizer.h"
#include "Workspace.h"

ExtrudePathNodeToolAction::ExtrudePathNodeToolAction(Workspace* a_workspace, Editor* a_editor)
{
    m_workspace = a_workspace;
    m_editor = a_editor;
}
ExtrudePathNodeToolAction::~ExtrudePathNodeToolAction()
{

}

bool ExtrudePathNodeToolAction::Interact(const glm::mat4& a_viewProj, const glm::vec3& a_pos, e_Axis a_axis, const Camera* a_camera, const glm::vec2& a_cursorPos, const glm::vec2& a_screenSize, PathModel* a_model)
{
    constexpr glm::mat4 iden = glm::identity<glm::mat4>();
    constexpr glm::vec4 vec4Zero = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    constexpr float scale = 0.25f;
    constexpr glm::vec3 scale3 = glm::vec3(scale);

    const glm::vec3 axis = AxisControl::GetAxis(a_axis);
    const glm::vec3 scaledAxis = axis * scale;

    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    if (glm::abs(glm::dot(up, axis)) >= 0.95f)
    {
        up = glm::vec3(0.0f, 0.0f, 1.0f);
    }
    const glm::vec3 right = glm::cross(up, axis);
    up = glm::cross(right, axis);

    const glm::mat4 rot = glm::mat4(glm::vec4(right, 0.0f), glm::vec4(up, 0.0f), glm::vec4(axis, 0.0f), vec4Zero);
    const glm::mat4 mat = a_viewProj * glm::translate(iden, a_pos + scaledAxis) * glm::scale(iden, scale3) * rot;

    const LocalModel* handle = TransformVisualizer::GetScaleHandle();

    if (SelectionControl::PointInMesh(mat, handle, a_cursorPos))
    {
        const glm::vec3 cPos = a_camera->GetScreenToWorld(glm::vec3(a_cursorPos, 0.9f), a_screenSize);
        const unsigned int nodeCount = m_editor->GetSelectedNodeCount();
        const unsigned int* indices = m_editor->GetSelectedNodesArray();

        Action* action = new ExtrudePathNodeAction(m_workspace, m_editor, indices, nodeCount, a_model, cPos, axis);
        if (m_workspace->PushAction(action))
        {
            m_editor->SetCurrentAction(action);
        }
        else
        {
            printf("Error extruding path node \n");

            delete action;
        }

        delete[] indices;

        return true;
    }

    return false;
}

bool ExtrudePathNodeToolAction::LeftClicked(Camera* a_camera, const glm::vec2& a_cursorPos, const glm::vec2& a_screenSize)
{
    const Object* object = m_workspace->GetSelectedObject();
    if (object != nullptr)
    {
        PathModel* model = object->GetPathModel();
        if (model != nullptr)
        {
            const glm::mat4 transformMat = object->GetGlobalMatrix();

            const PathNodeCluster* nodes = model->GetPathNodes();

            glm::vec3 pos = glm::vec3(0.0f);

            const std::list<unsigned int> selectedNodes = m_editor->GetSelectedNodes();
            for (auto iter = selectedNodes.begin(); iter != selectedNodes.end(); ++iter)
            {
                pos += nodes[*iter].Nodes[0].Node.GetPosition();
            }

            pos /= selectedNodes.size();

            const glm::vec4 fPos = transformMat * glm::vec4(pos, 1.0f);

            const glm::mat4 view = a_camera->GetView();
            const glm::mat4 proj = a_camera->GetProjection(a_screenSize);

            const glm::mat4 viewProj = proj * view;

            if (Interact(viewProj, fPos, Axis_X, a_camera, a_cursorPos, a_screenSize, model) ||
                Interact(viewProj, fPos, Axis_Y, a_camera, a_cursorPos, a_screenSize, model) ||
                Interact(viewProj, fPos, Axis_Z, a_camera, a_cursorPos, a_screenSize, model))
            {
                return true;    
            }
        }
    }

    return false;
}
bool ExtrudePathNodeToolAction::LeftDown(Camera* a_camera, const glm::vec2& a_cursorPos, const glm::vec2& a_screenSize)
{
    const glm::vec3 cWorldPos = a_camera->GetScreenToWorld(glm::vec3(a_cursorPos, 0.9f), a_screenSize);
    if (m_editor->GetCurrentActionType() == ActionType_ExtrudePathNode)
    {
        ExtrudePathNodeAction* action = (ExtrudePathNodeAction*)m_editor->GetCurrentAction();
        action->SetPosition(cWorldPos);

        action->Execute();

        return true;
    }

    return false;
}
bool ExtrudePathNodeToolAction::LeftReleased(Camera* a_camera, const glm::vec2& a_cursorPos, const glm::vec2& a_screenSize)
{
    if (m_editor->GetCurrentActionType() == ActionType_ExtrudePathNode)
    {
        m_editor->SetCurrentAction(nullptr);

        return true;
    }

    return false;
}

void ExtrudePathNodeToolAction::Draw(Camera* a_camera)
{
    const Object* object = m_workspace->GetSelectedObject();
    if (object != nullptr)
    {
        const PathModel* model = object->GetPathModel();
        if (model != nullptr)
        {
            const Transform* camTrans = a_camera->GetTransform();

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

            Gizmos::DrawScale(fPos, camTrans->Forward(), 0.25f);
        }
    }
}