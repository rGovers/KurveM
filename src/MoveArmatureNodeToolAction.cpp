#include "ToolActions/MoveArmatureNodeToolAction.h"

#include <glm/gtx/quaternion.hpp>

#include "Actions/TranslateObjectRelativeAction.h"
#include "Camera.h"
#include "EditorControls/Editor.h"
#include "Gizmos.h"
#include "LocalModel.h"
#include "SelectionControl.h"
#include "Transform.h"
#include "TransformVisualizer.h"
#include "Workspace.h"

MoveArmatureNodeToolAction::MoveArmatureNodeToolAction(Workspace* a_workspace, Editor* a_editor)
{
    m_workspace = a_workspace;
    m_editor = a_editor;
}
MoveArmatureNodeToolAction::~MoveArmatureNodeToolAction()
{

}

bool MoveArmatureNodeToolAction::Interact(const glm::mat4& a_viewProj, const glm::vec3& a_pos, e_Axis a_axis, const Camera* a_camera, const glm::vec2& a_cursorPos, const glm::vec2& a_screenSize)
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

    const LocalModel* handle = TransformVisualizer::GetTranslationHandle();

    if (SelectionControl::PointInMesh(mat, handle, a_cursorPos))
    {
        const glm::vec3 cPos = a_camera->GetScreenToWorld(glm::vec3(a_cursorPos, 0.9f), a_screenSize);

        const int objectCount = m_editor->GetSelectedArmatureObjectsCount();
        Object* const*objs = m_editor->GetSelectedArmatureObjectsArray();

        Action* action = new TranslateObjectRelativeAction(cPos, axis, objs, objectCount);

        if (m_workspace->PushAction(action))
        {
            m_editor->SetCurrentAction(action);
        }
        else
        {
            printf("Error moving armature node \n");

            delete action;
        }

        delete[] objs;

        return true;
    }

    return false;
}
bool MoveArmatureNodeToolAction::LeftClicked(Camera* a_camera, const glm::vec2& a_cursorPos, const glm::vec2& a_screenSize)
{
    const std::list<long long> selectedNodes = m_editor->GetSelectedArmatureNodes();

    glm::vec3 pos = glm::vec3(0);

    int nodeCount = 0;
    for (auto iter = selectedNodes.begin(); iter != selectedNodes.end(); ++iter)
    {
        const Object *obj = m_workspace->GetObject(*iter);

        if (obj != nullptr)
        {
            const glm::mat4 mat = obj->GetGlobalMatrix();

            pos += mat[3].xyz();
            ++nodeCount;
        }
    }

    pos /= nodeCount;

    const glm::mat4 view = a_camera->GetView();
    const glm::mat4 proj = a_camera->GetProjection(a_screenSize);
    const glm::mat4 viewProj = proj * view;

    if (!Interact(viewProj, pos, Axis_X, a_camera, a_cursorPos, a_screenSize))
    {
        if (!Interact(viewProj, pos, Axis_Y, a_camera, a_cursorPos, a_screenSize))
        {
            Interact(viewProj, pos, Axis_Z, a_camera, a_cursorPos, a_screenSize);
        }
    }

    return true;
}
bool MoveArmatureNodeToolAction::LeftDown(Camera* a_camera, const glm::vec2& a_cursorPos, const glm::vec2& a_screenSize)
{
    const glm::vec3 cWorldPos = a_camera->GetScreenToWorld(glm::vec3(a_cursorPos, 0.9f), a_screenSize);

    if (m_editor->GetCurrentActionType() == ActionType_TranslateObjectRelative)
    {
        TranslateObjectRelativeAction* action = (TranslateObjectRelativeAction*)m_editor->GetCurrentAction();
        action->SetTranslation(cWorldPos);

        action->Execute();

        return true;
    }

    return false;
}
bool MoveArmatureNodeToolAction::LeftReleased(Camera* a_camera, const glm::vec2& a_cursorPos, const glm::vec2& a_screenSize)
{
    if (m_editor->GetCurrentActionType() == ActionType_TranslateObjectRelative)
    {
        m_editor->SetCurrentAction(nullptr);

        return true;
    }

    return false;
}

void MoveArmatureNodeToolAction::Draw(Camera* a_camera)
{
    const Transform* camTransform = a_camera->GetTransform();
    const glm::mat4 viewInv = camTransform->ToMatrix();

    glm::vec3 pos = glm::vec3(0.0f);
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

    Gizmos::DrawTranslation(pos, viewInv[2], 0.25f);
}