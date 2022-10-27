#include "PropertiesWindows/CurvePWindow.h"

#include "Actions/SetCurveArmatureAction.h"
#include "Actions/SetCurveSmartStepAction.h"
#include "Actions/SetCurveStepsAction.h"
#include "CurveModel.h"
#include "Object.h"
#include "Workspace.h"

CurvePWindow::CurvePWindow(Workspace* a_workspace)
{
    m_workspace = a_workspace;
}
CurvePWindow::~CurvePWindow()
{

}

int CurvePWindow::GetArmatures(std::vector<Object*>* a_items, long long a_id, Object* a_object) const
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

e_ObjectPropertiesTab CurvePWindow::GetWindowType() const
{
    return ObjectPropertiesTab_Curve;
}

void CurvePWindow::Update() 
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
            int steps = glm::max(triSteps, 1);

            m_workspace->PushActionSet(new SetCurveStepsAction(m_workspace, curveObjects, curveObjectCount, steps), &steps, "Error setting curve steps");
        }

        bool stepAdjust = model->IsStepAdjusted();
        if (ImGui::Checkbox("Smart Step", &stepAdjust))
        {
            m_workspace->PushActionSet(new SetCurveSmartStepAction(m_workspace, curveObjects, curveObjectCount, stepAdjust), &stepAdjust, "Error setting curve smart step");
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

                const bool selected = i == index;
                if (ImGui::Selectable(name, selected))
                {
                    long long curID = -1;
                    if (obj != nullptr)
                    {
                        curID = obj->GetID();
                    }

                    m_workspace->PushActionSet(new SetCurveArmatureAction(m_workspace, curveObjects, curveObjectCount, curID), &curID, "Error setting curve armature");
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