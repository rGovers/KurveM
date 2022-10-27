#include "PropertiesWindows/ObjectPWindow.h"

#include "Actions/RenameObjectAction.h"
#include "Actions/RotateObjectAction.h"
#include "Actions/ScaleObjectAction.h"
#include "Actions/TranslateObjectAction.h"
#include "imgui.h"
#include "Object.h"
#include "Transform.h"
#include "Workspace.h"

ObjectPWindow::ObjectPWindow(Workspace* a_workspace)
{
    m_workspace = a_workspace;

    m_transform = nullptr;

    m_rotationMode = RotationMode_AxisAngle;
}
ObjectPWindow::~ObjectPWindow()
{

}

e_ObjectPropertiesTab ObjectPWindow::GetWindowType() const
{
    return ObjectPropertiesTab_Object;
}

void ObjectPWindow::RotationModeDisplay()
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

void ObjectPWindow::Update()
{
    Action* curAction = m_workspace->GetCurrentAction();

    Object** objs = m_workspace->GetSelectedObjectArray();

    Object* obj = m_workspace->GetSelectedObject();
    Transform* transform = obj->GetTransform();

    const unsigned int objectCount = m_workspace->GetSelectedObjectCount();

    if (objectCount == 1)
    {
        char* buff = new char[1024];
        const char* name = obj->GetName();

        if (name == nullptr)
        {
            name = "";
        }

        int size = (int)strlen(name);

        for (int i = 0; i <= size; ++i)
        {
            buff[i] = name[i];
        }

        if (ImGui::InputText("Name", buff, 1000))
        {
            m_workspace->PushActionSet(new RenameObjectAction(name, buff, obj), buff, "Error renaming object");
        }

        delete[] buff;

        ImGui::Separator();
    }

    if (m_transform != transform)
    {
        m_transform = transform;

        m_transformQuaternion = m_transform->Quaternion();
        m_transformAxisAngle = glm::vec4(glm::axis(m_transformQuaternion), glm::angle(m_transformQuaternion));
        m_transformEuler = glm::eulerAngles(m_transformQuaternion);
    }

    glm::vec3 translation = m_transform->Translation();
    if (ImGui::DragFloat3("Translation", (float*)&translation, 0.01f))
    {
        m_workspace->PushActionSet(new TranslateObjectAction(translation, objs, objectCount), &translation, "Error translating object");
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

            glm::quat q = glm::angleAxis(m_transformAxisAngle.w, axis);

            m_workspace->PushActionSet(new RotateObjectAction(q, objs, m_workspace->GetSelectedObjectCount()), &q, "Error rotating object");
        }

        break;
    }
    case RotationMode_Quaternion:
    {
        if (ImGui::DragFloat4("Quaternion", (float*)&m_transformQuaternion, 0.01f))
        {
            m_workspace->PushActionSet(new RotateObjectAction(m_transformQuaternion, objs, m_workspace->GetSelectedObjectCount()), &m_transformQuaternion, "Error rotating object");
        }

        break;
    }
    case RotationMode_EulerAngle:
    {
        if (ImGui::DragFloat3("Euler Angle", (float*)&m_transformEuler, 0.01f))
        {
            glm::quat q = glm::angleAxis(m_transformEuler.x, glm::vec3(1.0f, 0.0f, 0.0f)) * glm::angleAxis(m_transformEuler.y, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::angleAxis(m_transformEuler.z, glm::vec3(0.0f, 0.0f, 1.0f));

            m_workspace->PushActionSet(new RotateObjectAction(q, objs, m_workspace->GetSelectedObjectCount()), &q, "Error rotating object");

        }

        break;
    }
    }

    glm::vec3 scale = transform->Scale();
    if (ImGui::DragFloat3("Scale", (float *)&scale, 0.01f))
    {
        m_workspace->PushActionSet(new ScaleObjectAction(scale, objs, objectCount), &scale, "Error scaling object");
    }

    delete[] objs;
}