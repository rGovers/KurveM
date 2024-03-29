#include "Editors/AnimateEditor.h"

#include "Animation.h"
#include "Camera.h"
#include "ColorTheme.h"
#include "Gizmos.h"
#include "Object.h"
#include "Physics/CollisionShapes/MeshCollisionShape.h"
#include "PhysicsEngine.h"
#include "Transform.h"
#include "Workspace.h"

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
        a_object->DrawAnimator(m_editor->GetEditorDrawMode(), a_camera, a_winSize);

        const e_ObjectType objectType = a_object->GetObjectType();

        switch (objectType)
        {
        case ObjectType_ArmatureNode:
        {
            const glm::vec3 translation = a_object->GetGlobalTranslation();
            const glm::vec3 camFor = glm::normalize(camTransform->Forward());

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

void AnimateEditor::UpdateObject(Object* a_object)
{
    if (a_object != nullptr)
    {
        const Animation* animation = m_workspace->GetCurrentAnimation();
        if (animation != nullptr)
        {
            if (animation->ContainsObject(a_object))
            {
                const float time = m_editor->GetAnimationTime();

                Transform* animTransform = a_object->GetAnimationTransform();

                animTransform->Translation() = animation->GetTranslation(a_object, time);
                animTransform->Quaternion() = animation->GetRotation(a_object, time);
                animTransform->Scale() = animation->GetScale(a_object, time);
            }

            if (a_object->GetCollisionShapeType() == CollisionShapeType_Mesh)
            {
                MeshCollisionShape* shape = (MeshCollisionShape*)a_object->GetCollisionShape();
                
                shape->Tick();
            }

            const std::list<Object*> children = a_object->GetChildren();
            for (auto iter = children.begin(); iter != children.end(); ++iter)
            {
                UpdateObject(*iter);
            }
        }
    }
}

void AnimateEditor::Update(Camera* a_camera, const glm::vec2& a_cursorPos, const glm::vec2& a_screenSize, double a_delta)
{
    const std::list<Object*> objs = m_workspace->GetObjectList();

    for (auto iter = objs.begin(); iter != objs.end(); ++iter)
    {
        UpdateObject(*iter);
    }

    m_editor->GetPhysicsEngine()->Draw(a_camera);
}