#include "PropertiesWindows/PathPWindow.h"

#include "Actions/SetPathArmatureAction.h"
#include "Actions/SetPathPathStepsAction.h"
#include "Actions/SetPathShapeStepsAction.h"
#include "Object.h"
#include "PathModel.h"
#include "Workspace.h"

PathPWindow::PathPWindow(Workspace* a_workspace)
{
    m_workspace = a_workspace;
}
PathPWindow::~PathPWindow()
{

}

int PathPWindow::GetArmatures(std::vector<Object*>* a_items, long long a_id, Object* a_object) const
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

e_ObjectPropertiesTab PathPWindow::GetWindowType() const
{
    return ObjectPropertiesTab_Path;
}
void PathPWindow::Update()
{
    Object* obj = m_workspace->GetSelectedObject();

    if (obj != nullptr)
    {
        PathModel* model = obj->GetPathModel();
        if (model != nullptr)
        {
            const unsigned int objectCount = m_workspace->GetSelectedObjectCount();
            Object** objs = m_workspace->GetSelectedObjectArray();

            unsigned int pathObjectCount = 0;
            Object** pathObjs = new Object*[objectCount];
            for (unsigned int i = 0; i < objectCount; ++i)
            {
                if (objs[i]->GetObjectType() == ObjectType_PathModel)
                {
                    pathObjs[pathObjectCount++] = objs[i];
                }
            }
            
            int shapeSteps = model->GetShapeSteps();
            if (ImGui::InputInt("Shape Steps", &shapeSteps))
            {
                shapeSteps = glm::max(shapeSteps, 1);

                switch (m_workspace->GetCurrentActionType())
                {
                case ActionType_SetPathShapeSteps:
                {
                    SetPathShapeStepsAction* action = (SetPathShapeStepsAction*)m_workspace->GetCurrentAction();
                
                    action->SetSteps(shapeSteps);
                    action->Execute();

                    break;
                }
                default:
                {
                    Action* action = new SetPathShapeStepsAction(m_workspace, pathObjs, pathObjectCount, shapeSteps);
                    if (!m_workspace->PushAction(action))
                    {
                        printf("Error Setting Path Shape Steps \n");

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

            int pathSteps = model->GetPathSteps();
            if (ImGui::InputInt("Path Steps", &pathSteps))
            {
                pathSteps = glm::max(pathSteps, 1);

                switch (m_workspace->GetCurrentActionType())
                {
                case ActionType_SetPathPathSteps:
                {
                    SetPathPathStepsAction* action = (SetPathPathStepsAction*)m_workspace->GetCurrentAction();
                
                    action->SetSteps(pathSteps);
                    action->Execute();

                    break;
                }
                default:
                {
                    Action* action = new SetPathPathStepsAction(m_workspace, pathObjs, pathObjectCount, pathSteps);
                    if (!m_workspace->PushAction(action))
                    {
                        printf("Error Setting Path Path Steps \n");

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
                const int size = (int)items.size();

                for (int i = 0; i < size; ++i)
                {
                    name = "Null";

                    obj = items[i];
                    if (obj != nullptr)
                    {
                        name = obj->GetName();
                    }

                    if (ImGui::Selectable(name, i == index))
                    {
                        long long curID = -1;
                        if (obj != nullptr)
                        {
                            curID = obj->GetID();
                        }

                        if (m_workspace->GetCurrentActionType() == ActionType_SetPathArmature)
                        {
                            SetPathArmatureAction* action = (SetPathArmatureAction*)m_workspace->GetCurrentAction();

                            action->SetID(curID);
                            action->Execute();
                        }
                        else
                        {
                            Action* action = new SetPathArmatureAction(m_workspace, objs, objectCount, curID);
                            if (!m_workspace->PushAction(action))
                            {
                                printf("Error setting path armature \n");

                                delete action;
                            }
                            else
                            {
                                m_workspace->SetCurrentAction(action);
                            }
                        }
                    }
                }

                ImGui::EndCombo();   
            }

            delete[] objs;
            delete[] pathObjs;
        }
    }
}