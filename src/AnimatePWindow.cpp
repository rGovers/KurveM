#include "PropertiesWindows/AnimatePWindow.h"

#include "Actions/SetAnimationNodeAction.h"
#include "EditorControls/Editor.h"
#include "Workspace.h"

AnimatePWindow::AnimatePWindow(Workspace* a_workspace, Editor* a_editor)
{
    m_workspace = a_workspace;
    m_editor = a_editor;

    m_transform = nullptr;

    m_rotationMode = RotationMode_AxisAngle;
}
AnimatePWindow::~AnimatePWindow()
{

}

e_ObjectPropertiesTab AnimatePWindow::GetWindowType() const
{
    return ObjectPropertiesTab_Animate;
}

void AnimatePWindow::RotationModeDisplay()
{
    if (ImGui::BeginCombo("Rotation Mode", RotationModeString[m_rotationMode]))
    {
        for (int i = 0; i < RotationMode_End; ++i)
        {
            const bool selected = m_rotationMode == i;
            if (ImGui::Selectable(RotationModeString[i], selected))
            {
                m_rotationMode = (e_RotationMode)i;

                switch (m_workspace->GetCurrentActionType())
                {
                case ActionType_RotateObject:
                {
                    m_workspace->ClearCurrentAction();

                    break;
                }
                }

                m_node.Time = -1.0f;
                m_transform = nullptr;
            }

            if (selected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }

        ImGui::EndCombo();
    }
}

void AnimatePWindow::Update()
{
    Animation* animation = m_workspace->GetCurrentAnimation(); 
    Object* obj = m_workspace->GetSelectedObject();
    const float time = m_editor->GetSelectedTime();
    
    if (animation != nullptr)
    {
        const int referenceFramerate = animation->GetReferenceFramerate();
        const int selectedFrame = (int)(time * referenceFramerate);

        if (m_lastObject != obj || m_node.Time == -1 || m_lastTime != time)
        {
            m_node = animation->GetKeyNode(obj, selectedFrame);

            m_transformQuaternion = m_node.Rotation;
            m_transformAxisAngle = glm::vec4(glm::axis(m_transformQuaternion), glm::angle(m_transformQuaternion));
            m_transformEuler = glm::eulerAngles(m_transformQuaternion);

            m_lastTime = time;
            m_lastObject = obj;
        }

        AnimationNode node = animation->GetKeyNode(obj, selectedFrame);

        if (node.Time >= 0)
        {
            if (ImGui::DragFloat3("Translation", (float*)&node.Translation, 0.01f))
            {
                m_workspace->PushActionSet(new SetAnimationNodeAction(animation, obj, node), &node, "Failed to set animation node");
            }

            RotationModeDisplay();

            switch (m_rotationMode)
            {
            case RotationMode_AxisAngle:
            {
                if (ImGui::DragFloat4("Axis Angle", (float*)&m_transformAxisAngle, 0.01f))
                {
                    const float len = glm::length(m_transformAxisAngle.xyz());
                    glm::vec3 axis = m_transformAxisAngle.xyz() / len;
                    if (len <= 0)
                    {
                        axis = glm::vec3(0.0f, 1.0f, 0.0f);
                    }

                    node.Rotation = glm::angleAxis(m_transformAxisAngle.w, axis);

                    m_workspace->PushActionSet(new SetAnimationNodeAction(animation, obj, node), &node, "Failed to set animation node");
                }

                break;
            }
            case RotationMode_Quaternion:
            {
                if (ImGui::DragFloat4("Quaternion", (float*)&m_transformQuaternion, 0.01f))
                {
                    node.Rotation = glm::normalize(m_transformQuaternion);

                    m_workspace->PushActionSet(new SetAnimationNodeAction(animation, obj, node), &node, "Failed to set animation node");
                }

                break;
            }
            case RotationMode_EulerAngle:
            {
                if (ImGui::DragFloat3("Euler Angle", (float*)&m_transformEuler, 0.01f))
                {
                    node.Rotation = glm::angleAxis(m_transformEuler.x, glm::vec3(1.0f, 0.0f, 0.0f)) * glm::angleAxis(m_transformEuler.y, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::angleAxis(m_transformEuler.z, glm::vec3(0.0f, 0.0f, 1.0f));

                    m_workspace->PushActionSet(new SetAnimationNodeAction(animation, obj, node), &node, "Failed to set animation node");
                }

                break;
            }
            }

            if (ImGui::DragFloat3("Scale", (float*)&node.Scale, 0.01f))
            {
                m_workspace->PushActionSet(new SetAnimationNodeAction(animation, obj, node), &node, "Failed to set animation node");
            }
        }
    }
}