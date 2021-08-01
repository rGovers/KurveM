#include "Windows/PropertiesWindow.h"

#include "Actions/RenameObjectAction.h"
#include "Actions/RotateObjectAction.h"
#include "Actions/ScaleObjectAction.h"
#include "Actions/SetAnimationNodeAction.h"
#include "Actions/SetCurveArmatureAction.h"
#include "Actions/SetCurveSmartStepAction.h"
#include "Actions/SetCurveStepsAction.h"
#include "Actions/TranslateObjectAction.h"
#include "imgui.h"
#include "ImGuiExt.h"
#include "LongTasks/TriangulateCurveLongTask.h"
#include "Object.h"
#include "Transform.h"
#include "Workspace.h"

const char* RotationMode_String[] = { "Axis Angle", "Quaternion", "Euler Angle" };

#define ANIMATE_TOOLTIP "Contains object animation settings"
#define OBJECT_TOOLTIP "Contains object settings"
#define CURVE_TOOLTIP "Contains curve model settings"

PropertiesWindow::PropertiesWindow(Workspace* a_workspace, Editor* a_editor)
{
    m_workspace = a_workspace;
    m_editor = a_editor;

    m_transform = nullptr;

    m_rotationMode = RotationMode_AxisAngle;
    m_propertiesMode = ObjectPropertiesTab_Object;

    m_lastObject = nullptr;
    m_lastTime = -1;
    m_node.Time = -1;
}
PropertiesWindow::~PropertiesWindow()
{

}

void PropertiesWindow::PushRotation(const glm::quat& a_quat)
{
    switch (m_workspace->GetCurrentActionType())
    {
    case ActionType_RotateObject:
    {
        RotateObjectAction* action = (RotateObjectAction*)m_workspace->GetCurrentActionType();

        action->SetRotation(a_quat);
        action->Execute();

        break;
    }
    default:
    {
        Object** objs = m_workspace->GetSelectedObjectArray();

        Action* action = new RotateObjectAction(a_quat, objs, m_workspace->GetSelectedObjectCount());
        if (!m_workspace->PushAction(action))
        {
            printf("Error rotating object \n");

            delete action;
        }

        delete[] objs;

        break;
    }
    }
}
void PropertiesWindow::PushAnimationNode(Animation* a_animation, Object* a_obj, const AnimationNode& a_node)
{
    switch (m_workspace->GetCurrentActionType())
    {
    case ActionType_SetAnimationNode:
    {
        SetAnimationNodeAction* action = (SetAnimationNodeAction*)m_workspace->GetCurrentAction();

        action->SetNode(a_node);
        action->Execute();

        break;
    }
    default:
    {
        Action* action = new SetAnimationNodeAction(a_animation, a_obj, a_node);
        if (!m_workspace->PushAction(action))
        {
            printf("Failed to set animation node \n");

            delete action;
        }
        else
        {
            m_workspace->SetCurrentAction(action);
        }

        break;
    }
    }
}

void PropertiesWindow::RotationModeDisplay()
{
    if (ImGui::BeginCombo("Rotation Mode", RotationMode_String[m_rotationMode]))
    {
        for (int i = 0; i < RotationMode_End; ++i)
        {
            const bool selected = m_rotationMode == i;
            if (ImGui::Selectable(RotationMode_String[i], selected))
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

                m_node.Time = -1;
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

void PropertiesWindow::PropertiesTabButton(const char* a_label, const char* a_path, e_ObjectPropertiesTab a_propertiesTab, const char* a_tooltip)
{
    if (ImGuiExt::ImageToggleButton(a_label, a_path, m_propertiesMode == a_propertiesTab, glm::vec2(16, 16)))
    {
        m_propertiesMode = a_propertiesTab;
    }

    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();

        ImGui::Text(a_label);

        ImGui::Separator();

        ImGui::Text(a_tooltip);

        ImGui::EndTooltip();
    }
}

int GetArmatures(std::vector<Object*>* a_items, long long a_id, Object* a_object)
{
    int index = 0;

    if (a_object->GetID() == a_id)
    {
        index = a_items->size();
    }

    const e_ObjectType objType = a_object->GetObjectType();
    if (objType == ObjectType_Armature)
    {
        a_items->emplace_back(a_object);
    }

    const std::list<Object*> children = a_object->GetChildren();

    for (auto iter = children.begin(); iter != children.end(); ++iter)
    {
        const int otherIndex = GetArmatures(a_items, a_id, *iter);

        if (otherIndex != 0)
        {
            index = otherIndex;
        }
    }

    return index;
}

void PropertiesWindow::ObjectTab()
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

        int size = strlen(name);

        for (int i = 0; i <= size; ++i)
        {
            buff[i] = name[i];
        }

        if (ImGui::InputText("Name", buff, 1000))
        {
            switch (m_workspace->GetCurrentActionType())
            {
            case ActionType_RenameObject:
            {
                RenameObjectAction* action = (RenameObjectAction*)curAction;

                action->SetNewName(buff);
                action->Execute();

                break;
            }
            default:
            {
                Action* action = new RenameObjectAction(name, buff, obj);

                if (!m_workspace->PushAction(action))
                {
                    printf("Error Renaming Object \n");

                    delete action;
                }
                else
                {
                    m_workspace->SetCurrentAction(action);
                }

                break;
            }
            }
        }

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
        switch (m_workspace->GetCurrentActionType())
        {
        case ActionType_TranslateObject:
        {
            TranslateObjectAction* action = (TranslateObjectAction*)curAction;

            action->SetTranslation(translation);
            action->Execute();

            break;
        }
        default:
        {
            Action* action = new TranslateObjectAction(translation, objs, objectCount);
            if (!m_workspace->PushAction(action))
            {
                printf("Error Renaming Object \n");

                delete action;
            }
            else
            {
                m_workspace->SetCurrentAction(action);
            }

            break;
        }
        }
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
                axis = glm::vec3(0, 1, 0);
            }

            PushRotation(glm::angleAxis(m_transformAxisAngle.w, axis));
        }

        break;
    }
    case RotationMode_Quaternion:
    {
        if (ImGui::DragFloat4("Quaternion", (float*)&m_transformQuaternion, 0.01f))
        {
            PushRotation(glm::normalize(m_transformQuaternion));
        }

        break;
    }
    case RotationMode_EulerAngle:
    {
        if (ImGui::DragFloat3("Euler Angle", (float*)&m_transformEuler, 0.01f))
        {
            PushRotation(glm::angleAxis(m_transformEuler.x, glm::vec3(1, 0, 0)) * glm::angleAxis(m_transformEuler.y, glm::vec3(0, 1, 0)) * glm::angleAxis(m_transformEuler.z, glm::vec3(0, 0, 1)));
        }

        break;
    }
    }

    glm::vec3 scale = m_transform->Scale();
    if (ImGui::DragFloat3("Scale", (float *)&scale, 0.01f))
    {
        switch (m_workspace->GetCurrentActionType())
        {
        case ActionType_ScaleObject:
        {
            ScaleObjectAction* action = (ScaleObjectAction*)curAction;

            action->SetScale(scale);
            action->Execute();

            break;
        }
        default:
        {
            Action* action = new ScaleObjectAction(scale, objs, objectCount);
            if (!m_workspace->PushAction(action))
            {
                printf("Error Renaming Object \n");

                delete action;
            }
            else
            {
                m_workspace->SetCurrentAction(action);
            }

            break;
        }
        }
    }

    delete[] objs;
}
void PropertiesWindow::AnimateTab()
{
    Animation* animation = m_workspace->GetCurrentAnimation();
    Object* obj = m_workspace->GetSelectedObject();
    const float time = m_editor->GetSelectedTime();

    if (animation != nullptr)
    {
        if (m_lastObject != obj || m_node.Time != -1 || m_lastTime != time)
        {
            m_node = animation->GetNode(obj, time);

            m_nodeQuaternion = m_node.Rotation;
            m_nodeAxisAngle = glm::vec4(glm::axis(m_nodeQuaternion), glm::angle(m_nodeQuaternion));
            m_nodeEuler = glm::eulerAngles(m_nodeQuaternion);

            m_lastTime = time;
            m_lastObject = obj;
        }

        AnimationNode node = animation->GetNode(obj, time);

        if (node.Time >= 0)
        {
            if (ImGui::DragFloat3("Translation", (float*)&node.Translation, 0.01f))
            {
                PushAnimationNode(animation, obj, node);
            }

            RotationModeDisplay();

            switch (m_rotationMode)
            {
            case RotationMode_AxisAngle:
            {
                if (ImGui::DragFloat4("Axis Angle", (float*)&m_nodeAxisAngle, 0.01f))
                {
                    const float len = glm::length(m_nodeAxisAngle.xyz());
                    glm::vec3 axis = m_nodeAxisAngle.xyz() / len;
                    if (len <= 0)
                    {
                        axis = glm::vec3(0, 1, 0);
                    }

                    node.Rotation = glm::angleAxis(m_nodeAxisAngle.w, axis);

                    PushAnimationNode(animation, obj, node);
                }

                break;
            }
            case RotationMode_Quaternion:
            {
                if (ImGui::DragFloat4("Quaternion", (float*)&m_nodeQuaternion, 0.01f))
                {
                    node.Rotation = glm::normalize(m_nodeQuaternion);

                    PushAnimationNode(animation, obj, node);
                }

                break;
            }
            case RotationMode_EulerAngle:
            {
                if (ImGui::DragFloat3("Euler Angle", (float*)&m_nodeEuler, 0.01f))
                {
                    node.Rotation = glm::angleAxis(m_nodeEuler.x, glm::vec3(1, 0, 0)) * glm::angleAxis(m_nodeEuler.y, glm::vec3(0, 1, 0)) * glm::angleAxis(m_nodeEuler.z, glm::vec3(0, 0, 1));

                    PushAnimationNode(animation, obj, node);
                }

                break;
            }
            }

            if (ImGui::DragFloat3("Scale", (float*)&node.Scale, 0.01f))
            {
                PushAnimationNode(animation, obj, node);
            }
        }
    }
    
}
void PropertiesWindow::CurveTab()
{
    Object* obj = m_workspace->GetSelectedObject();

    CurveModel* model = obj->GetCurveModel();
    if (model != nullptr)
    {
        const unsigned int objectCount = m_workspace->GetSelectedObjectCount();
        Object** objs = m_workspace->GetSelectedObjectArray();

        Object** curveObjects = new Object*[objectCount];
        int curveObjectCount = 0;
        for (int i = 0; i < objectCount; ++i)
        {
            if (objs[i]->GetObjectType() == ObjectType_CurveModel)
            {
                curveObjects[curveObjectCount++] = objs[i];
            }
        }

        int triSteps = model->GetSteps();
        if (ImGui::InputInt("Curve Resolution", &triSteps))
        {
            const int steps = glm::max(triSteps, 1);

            switch (m_workspace->GetCurrentActionType())
            {
            case ActionType_SetCurveSteps:
            {
                SetCurveStepsAction* action = (SetCurveStepsAction*)m_workspace->GetCurrentAction();

                action->SetSteps(steps);
                action->Execute();

                break;
            }
            default:
            {
                Action *action = new SetCurveStepsAction(m_workspace, curveObjects, curveObjectCount, steps);
                if (!m_workspace->PushAction(action))
                {
                    printf("Error Setting Curve Steps \n");

                    delete action;
                }
                else
                {
                    m_workspace->SetCurrentAction(action);
                }

                break;
            }
            }
        }

        bool stepAdjust = model->IsStepAdjusted();
        if (ImGui::Checkbox("Smart Step", &stepAdjust))
        {
            switch (m_workspace->GetCurrentActionType())
            {
            case ActionType_SetCurveSmartStep:
            {
                SetCurveSmartStepAction* action = (SetCurveSmartStepAction*)m_workspace->GetCurrentAction();

                action->SetValue(stepAdjust);
                action->Execute();

                break;
            }
            default:
            {
                Action* action = new SetCurveSmartStepAction(m_workspace, curveObjects, curveObjectCount, stepAdjust);
                if (!m_workspace->PushAction(action))
                {
                    printf("Error Setting Curve Smart Step \n");

                    delete action;
                }
                else
                {
                    m_workspace->SetCurrentAction(action);
                }  

                break;
            }
            }
        }

        int index = 0;
        std::vector<Object*> items;

        items.emplace_back(nullptr);

        const long long id = model->GetArmatureID();

        const std::list<Object*> objects = m_workspace->GetObjectList();
        for (auto iter = objects.begin(); iter != objects.end(); ++iter)
        {
            const int otherIndex = GetArmatures(&items, id, *iter);
            if (otherIndex != 0)
            {
                index = otherIndex;
            }
        }

        const Object* obj = items[index];

        const char* name = "Null";

        if (obj != nullptr)
        {
            name = obj->GetName();
        }

        if (ImGui::BeginCombo("Armature", name))
        {
            const int size = items.size();

            for (int i = 0; i < size; ++i)
            {
                name = "Null";

                obj = items[i];
                if (obj != nullptr)
                {
                    name = obj->GetName();
                }

                long long curID = -1;
                if (obj != nullptr)
                {
                    curID = obj->GetID();
                }

                const bool selected = i == index;
                if (ImGui::Selectable(name, selected))
                {
                    switch (m_workspace->GetCurrentActionType())
                    {
                    case ActionType_SetCurveArmature:
                    {
                        SetCurveArmatureAction* action = (SetCurveArmatureAction*)m_workspace->GetCurrentAction();

                        action->SetID(curID);
                        action->Execute();

                        break;
                    }
                    default:
                    {
                        Action* action = new SetCurveArmatureAction(m_workspace, curveObjects, curveObjectCount, curID);
                        if (!m_workspace->PushAction(action))
                        {
                            printf("Error Setting Armature \n");

                            delete action;
                        }
                        else
                        {
                            m_workspace->SetCurrentAction(action);
                        }

                        break;
                    }
                    }
                }

                if (selected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }

            ImGui::EndCombo();
        }

        delete[] objs;
        delete[] curveObjects;
    }
}

void PropertiesWindow::Update(double a_delta)
{
    if (ImGui::Begin("Properties"))
    {
        const int objectCount = m_workspace->GetSelectedObjectCount();

        if (objectCount > 0)
        {
            Object* obj = m_workspace->GetSelectedObject();
            Transform* transform = obj->GetTransform();

            ImGui::BeginGroup();

            const e_ObjectType objectType = obj->GetObjectType();

            PropertiesTabButton("Object", "Textures/PROPERTIES_OBJECT.png", ObjectPropertiesTab_Object, OBJECT_TOOLTIP);

            switch (objectType)
            {
            case ObjectType_ArmatureNode:
            {
                if (m_editor->GetEditorMode() == EditorMode_Animate)
                {
                    PropertiesTabButton("Animate", "Textures/PROPERTIES_ANIMATE.png", ObjectPropertiesTab_Animate, ANIMATE_TOOLTIP);
                }

                break;
            }
            case ObjectType_CurveModel:
            {
                PropertiesTabButton("Curve", "Textures/PROPERTIES_CURVE.png", ObjectPropertiesTab_Curve, CURVE_TOOLTIP);

                break;
            }
            }

            ImGui::EndGroup();

            ImGui::SameLine();

            ImGui::BeginGroup();

            switch (m_propertiesMode)
            {
            case ObjectPropertiesTab_Animate:
            {
                AnimateTab();

                break;
            }
            case ObjectPropertiesTab_Curve:
            {
                CurveTab();

                break;
            }
            case ObjectPropertiesTab_Object:
            {
                ObjectTab();

                break;
            }
            }

            ImGui::EndGroup();
        }
    }
    
    ImGui::End();
}