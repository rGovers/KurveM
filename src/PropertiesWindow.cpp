#include "Windows/PropertiesWindow.h"

#include "Actions/RenameObjectAction.h"
#include "Actions/RotateObjectAction.h"
#include "Actions/ScaleObjectAction.h"
#include "Actions/TranslateObjectAction.h"
#include "imgui.h"
#include "LongTasks/TriangulateCurveLongTask.h"
#include "Object.h"
#include "Transform.h"
#include "Workspace.h"

const char* RotationMode_String[] = { "Axis Angle", "Quaternion", "Euler Angle" };

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

void PropertiesWindow::Update(double a_delta)
{
    if (ImGui::Begin("Properties"))
    {
        const int size = m_workspace->GetSelectedObjectCount();

        if (size > 0)
        {
            Object* obj = m_workspace->GetSelectedObject();
            Transform* transform = obj->GetTransform();

            ImGui::BeginGroup();

            if (ImGui::Button("Object"))
            {
                m_propertiesMode = ObjectPropertiesTab_Object;
            }

            if (ImGui::Button("Curve"))
            {
                m_propertiesMode = ObjectPropertiesTab_Curve;
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
                if (size == 1)
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
                        Action* action = new TranslateObjectAction(pos, objs, size);
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
                        PushRotation(glm::angleAxis(m_axisAngle.w, glm::normalize(m_axisAngle.xyz())));
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
                        Action* action = new ScaleObjectAction(scale, objs, size);
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
                    int triSteps = model->GetSteps();
                    if (ImGui::InputInt("Curve Resolution", &triSteps))
                    {
                        model->SetSteps(glm::max(triSteps, 1));
                        m_workspace->PushLongTask(new TriangulateCurveLongTask(model));
                    }

                    bool stepAdjust = model->IsStepAdjusted();
                    if (ImGui::Checkbox("Smart Step", &stepAdjust))
                    {
                        model->SetStepAdjust(stepAdjust);
                        m_workspace->PushLongTask(new TriangulateCurveLongTask(model));
                    }
                }

                break;
            }
            }

            ImGui::EndGroup();
        }
    }
    
    ImGui::End();
}