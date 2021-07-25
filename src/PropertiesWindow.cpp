#include "Windows/PropertiesWindow.h"

#include "Actions/RenameObjectAction.h"
#include "Actions/RotateObjectAction.h"
#include "Actions/ScaleObjectAction.h"
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

#define OBJECT_TOOLTIP "Contains object settings"
#define CURVE_TOOLTIP "Contains curve model settings"

PropertiesWindow::PropertiesWindow(Workspace* a_workspace, Editor* a_editor)
{
    m_workspace = a_workspace;
    m_editor = a_editor;

    m_transform = nullptr;

    m_rotationMode = RotationMode_AxisAngle;
    m_propertiesMode = ObjectPropertiesTab_Object;
}
PropertiesWindow::~PropertiesWindow()
{

}

void PropertiesWindow::PushRotation(const glm::quat& a_quat)
{
    Action* action = m_workspace->GetCurrentAction();

    if (action != nullptr && action->GetActionType() == ActionType_RotateObject)
    {
        RotateObjectAction* action = (RotateObjectAction*)action;

        action->SetRotation(a_quat);
        action->Execute();
    }
    else 
    {
        Object** objs = m_workspace->GetSelectedObjectArray();

        Action* action = new RotateObjectAction(a_quat, objs, m_workspace->GetSelectedObjectCount());
        if (!m_workspace->PushAction(action))
        {
            printf("Error rotating object \n");

            delete action;
        }

        delete[] objs;
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

            PropertiesTabButton("Object", "Textures/PROPERTIES_OBJECT.png", ObjectPropertiesTab_Object, OBJECT_TOOLTIP);

            if (obj->GetObjectType() == ObjectType_CurveModel)
            {
                PropertiesTabButton("Curve", "Textures/PROPERTIES_CURVE.png", ObjectPropertiesTab_Curve, CURVE_TOOLTIP);
            }

            ImGui::EndGroup();

            ImGui::SameLine();

            ImGui::BeginGroup();

            Object** objs = m_workspace->GetSelectedObjectArray();

            Action* curAction = m_workspace->GetCurrentAction();

            switch (m_propertiesMode)
            {
            case ObjectPropertiesTab_Object:
            {
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
                        if (curAction != nullptr && curAction->GetActionType() == ActionType_RenameObject)
                        {
                            RenameObjectAction* action = (RenameObjectAction*)curAction;

                            action->SetNewName(buff);
                            action->Execute();
                        }
                        else
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
                        }
                    }

                    ImGui::Separator();
                }

                glm::vec3 pos = transform->Translation();
                if (ImGui::DragFloat3("Position", (float*)&pos, 0.01f))
                {
                    if (curAction != nullptr && curAction->GetActionType() == ActionType_TranslateObject)
                    {
                        TranslateObjectAction* action = (TranslateObjectAction*)curAction;

                        action->SetTranslation(pos);
                        action->Execute();
                    }
                    else
                    {
                        Action* action = new TranslateObjectAction(pos, objs, objectCount);
                        if (!m_workspace->PushAction(action))
                        {
                            printf("Error Renaming Object \n");

                            delete action;
                        }  
                        else
                        {
                            m_workspace->SetCurrentAction(action);
                        }
                    }
                }

                if (ImGui::BeginCombo("Rotation Mode", RotationMode_String[m_rotationMode]))
                {
                    for (int i = 0; i < RotationMode_End; ++i)
                    {
                        const bool selected = m_rotationMode == i;
                        if (ImGui::Selectable(RotationMode_String[i], selected))
                        {
                            m_rotationMode = (e_RotationMode)i;

                            if (curAction != nullptr && curAction->GetActionType() == ActionType_RotateObject)
                            {
                                m_workspace->ClearCurrentAction();
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

                if (m_transform != transform)
                {
                    m_transform = transform;
                    
                    m_quaternion = m_transform->Quaternion();
                    m_axisAngle = glm::vec4(glm::axis(m_quaternion), glm::angle(m_quaternion));
                    m_euler = glm::eulerAngles(m_quaternion);
                }

                switch (m_rotationMode)
                {
                case RotationMode_AxisAngle:
                {
                    if (ImGui::DragFloat4("Axis Angle", (float*)&m_axisAngle, 0.01f))
                    {
                        const float len = glm::length(m_axisAngle.xyz()); 
                        glm::vec3 axis = m_axisAngle.xyz() / len;
                        if (len <= 0)
                        {
                            axis = glm::vec3(0, 1, 0);
                        }

                        PushRotation(glm::angleAxis(m_axisAngle.w, axis));
                    }

                    break;
                }
                case RotationMode_Quaternion:
                {
                    if (ImGui::DragFloat4("Quaternion", (float*)&m_quaternion, 0.01f))
                    {
                        PushRotation(glm::normalize(m_quaternion));
                    }

                    break;
                }
                case RotationMode_EulerAngle:
                {
                    if (ImGui::DragFloat3("Euler Angle", (float*)&m_euler, 0.01f))
                    {
                        PushRotation(glm::angleAxis(m_euler.x, glm::vec3(1, 0, 0)) * glm::angleAxis(m_euler.y, glm::vec3(0, 1, 0)) * glm::angleAxis(m_euler.z, glm::vec3(0, 0, 1)));
                    }

                    break;
                }
                }

                glm::vec3 scale = transform->Scale();
                if (ImGui::DragFloat3("Scale", (float*)&scale, 0.01f))
                {
                    if (curAction != nullptr && curAction->GetActionType() == ActionType_ScaleObject)
                    {
                        ScaleObjectAction* action = (ScaleObjectAction*)curAction;

                        action->SetScale(scale);
                        action->Execute();
                    }
                    else
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
                    }
                }

                break;
            }
            case ObjectPropertiesTab_Curve:
            {
                CurveModel* model = obj->GetCurveModel();
                if (model != nullptr)
                {
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

                        if (curAction != nullptr && curAction->GetActionType() == ActionType_SetCurveStepsAction)
                        {
                            SetCurveStepsAction* action = (SetCurveStepsAction*)curAction;

                            action->SetSteps(steps);
                            action->Execute();
                        }
                        else
                        {
                            Action* action = new SetCurveStepsAction(m_workspace, curveObjects, curveObjectCount, steps);
                            if (!m_workspace->PushAction(action))
                            {
                                printf("Error Setting Curve Steps \n");

                                delete action;
                            }  
                            else
                            {
                                m_workspace->SetCurrentAction(action);
                            }
                        }
                    }

                    bool stepAdjust = model->IsStepAdjusted();
                    if (ImGui::Checkbox("Smart Step", &stepAdjust))
                    {
                        if (curAction != nullptr && curAction->GetActionType() == ActionType_SetCurveSmartStepAction)
                        {
                            SetCurveSmartStepAction* action = (SetCurveSmartStepAction*)curAction;

                            action->SetValue(stepAdjust);
                            action->Execute();
                        }
                        else
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
                                if (curAction != nullptr && curAction->GetActionType() == ActionType_SetCurveArmatureAction)
                                {
                                    SetCurveArmatureAction* action = (SetCurveArmatureAction*)curAction;

                                    action->SetID(curID);
                                    action->Execute();
                                }
                                else
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
                                }
                            }

                            if (selected)
                            {
                                ImGui::SetItemDefaultFocus();
                            }
                        }

                        ImGui::EndCombo();
                    }

                    delete[] curveObjects;
                }

                break;
            }
            }

            ImGui::EndGroup();
        }
    }
    
    ImGui::End();
}