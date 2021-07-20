#include "Editors/ObjectEditor.h"

#include "Actions/TranslateObjectRelativeAction.h"
#include "Camera.h"
#include "Editor.h"
#include "Gizmos.h"
#include "Object.h"
#include "SelectionControl.h"
#include "Transform.h"
#include "Workspace.h"

ObjectEditor::ObjectEditor(Editor* a_editor, Workspace* a_workspace)
{
    m_editor = a_editor;
    m_workspace = a_workspace;
}
ObjectEditor::~ObjectEditor()
{

}

e_EditorMode ObjectEditor::GetEditorMode()
{
    return EditorMode_Object;
}

bool ObjectEditor::IsInteractingTransform(Camera* a_camera, const glm::vec3& a_pos, const glm::vec3& a_axis, const glm::vec2& a_cursorPos, const glm::vec2& a_screenSize, const glm::mat4& a_viewProj)
{
    if (SelectionControl::PointInPoint(a_viewProj, a_cursorPos, 0.025f, a_pos + a_axis * 0.5f))
    {
        const glm::vec3 cPos = a_camera->GetScreenToWorld(glm::vec3(a_cursorPos, 0.9f), (int)a_screenSize.x, (int)a_screenSize.y);

        const unsigned int objectCount = m_workspace->GetSelectedObjectCount();
        Object** objs = m_workspace->GetSelectedObjectArray();

        switch (m_workspace->GetToolMode())
        {
        case ToolMode_Translate:
        {
            Action* action = new TranslateObjectRelativeAction(cPos, a_axis, objs, objectCount);
            if (!m_workspace->PushAction(action))
            {
                printf("Error moving object \n");

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
    }
}

void ObjectEditor::DrawObject(Camera* a_camera, Object* a_object, const glm::vec2& a_winSize)
{
    a_object->DrawBase(a_camera, a_winSize);
}

void ObjectEditor::LeftClicked(Camera* a_camera, const glm::vec2& a_cursorPos, const glm::vec2& a_winSize)
{
    const std::list<Object*> selectedObjects = m_workspace->GetSelectedObjects();

    glm::vec3 pos = glm::vec3(0);

    for (auto iter = selectedObjects.begin(); iter != selectedObjects.end(); ++iter)
    {
        const glm::mat4 mat = (*iter)->GetGlobalMatrix();

        pos += mat[3].xyz();
    }

    pos /= selectedObjects.size();

    const glm::mat4 view = a_camera->GetView();
    const glm::mat4 proj = a_camera->GetProjection((int)a_winSize.x, (int)a_winSize.y);

    const glm::mat4 viewProj = proj * view;

    IsInteractingTransform(a_camera, pos, glm::vec3(0, 0, 1), a_cursorPos, a_winSize, viewProj);
    IsInteractingTransform(a_camera, pos, glm::vec3(0, 1, 0), a_cursorPos, a_winSize, viewProj);
    IsInteractingTransform(a_camera, pos, glm::vec3(1, 0, 0), a_cursorPos, a_winSize, viewProj);
}
void ObjectEditor::LeftDown(double a_delta, Camera* a_camera, const glm::vec2& a_start, const glm::vec2& a_currentPos, const glm::vec2& a_winSize)
{

}
void ObjectEditor::LeftReleased(Camera* a_camera, const glm::vec2& a_start, const glm::vec2& a_end, const glm::vec2& a_winSize)
{

}

void ObjectEditor::Update(Camera* a_camera, const glm::vec2& a_cursorPos, const glm::vec2& a_winSize, double a_delta)
{
    Transform* camTransform = a_camera->GetTransform();
    const glm::mat4 viewInv = camTransform->ToMatrix();

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

        const e_ToolMode toolMode = m_workspace->GetToolMode();
        switch (toolMode)
        {
        case ToolMode_Scale:
        {
            Gizmos::DrawScale(pos, viewInv[2], 0.5f);

            break;
        }
        default:
        {
            Gizmos::DrawTranslation(pos, viewInv[2], 0.5f);

            break;
        }
        }
    }
}