#include "Editors/AnimateEditor.h"

#include "Camera.h"
#include "ColorTheme.h"
#include "Gizmos.h"
#include "Object.h"
#include "Transform.h"

AnimateEditor::AnimateEditor(Editor* a_editor, Workspace* a_workspace)
{
    m_editor = a_editor;
    m_workspace = a_workspace;
}
AnimateEditor::~AnimateEditor()
{

}

e_EditorMode AnimateEditor::GetEditorMode()
{
    return EditorMode_Animate;
}

void AnimateEditor::DrawObject(Camera* a_camera, Object* a_object, const glm::vec2& a_winSize)
{
    const Transform* camTransform = a_camera->GetTransform();
    if (camTransform != nullptr)
    {
        a_object->DrawBase(a_camera, a_winSize);

        const e_ObjectType objectType = a_object->GetObjectType();

        const glm::mat4 camMat = camTransform->ToMatrix();

        const glm::vec3 camFor = glm::normalize(camMat[2]);

        switch (objectType)
        {
        case ObjectType_ArmatureNode:
        {
            const glm::vec3 translation = a_object->GetGlobalTranslation();

            Gizmos::DrawCircleFilled(translation, camFor, 0.01f, 20, ColorTheme::Active);

            break;
        }
        }
    }
}

void AnimateEditor::LeftClicked(Camera* a_camera, const glm::vec2& a_cursorPos, const glm::vec2& a_winSize)
{

}
void AnimateEditor::LeftDown(double a_delta, Camera* a_camera, const glm::vec2& a_start, const glm::vec2& a_end, const glm::vec2& a_winSize)
{

}
void AnimateEditor::LeftReleased(Camera* a_camera, const glm::vec2& a_start, const glm::vec2& a_end, const glm::vec2& a_winSize)
{

}

void AnimateEditor::Update(Camera* a_camera, const glm::vec2& a_cursorPos, const glm::vec2& a_screenSize, double a_delta)
{

}