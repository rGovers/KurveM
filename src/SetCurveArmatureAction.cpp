#include "Actions/SetCurveArmatureAction.h"

#include "CurveModel.h"
#include "LongTasks/TriangulateCurveLongTask.h"
#include "Object.h"
#include "Workspace.h"

SetCurveArmatureAction::SetCurveArmatureAction(Workspace* a_workspace, Object** a_objects, unsigned int a_objectCount, long long a_id)
{
    m_workspace = a_workspace;

    m_objectCount = a_objectCount;

    m_newID = a_id;

    m_oldID = new long long[m_objectCount];
    m_objects = new Object*[m_objectCount];

    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        m_objects[i] = a_objects[i];

        const CurveModel* model = m_objects[i]->GetCurveModel();
        if (model != nullptr)
        {
            m_oldID[i] = model->GetArmatureID();
        }
    }
}
SetCurveArmatureAction::~SetCurveArmatureAction()
{
    delete[] m_oldID;
    delete[] m_objects;
}

e_ActionType SetCurveArmatureAction::GetActionType() 
{
    return ActionType_SetCurveArmatureAction;
}

bool SetCurveArmatureAction::Redo()
{
    return Execute();
}

bool SetCurveArmatureAction::Execute()
{
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        CurveModel* model = m_objects[i]->GetCurveModel();
        if (model != nullptr)
        {
            model->SetArmature(m_newID);

            m_workspace->PushLongTask(new TriangulateCurveLongTask(model));
        }
    }

    return true;
}
bool SetCurveArmatureAction::Revert()
{
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        CurveModel* model = m_objects[i]->GetCurveModel();
        if (model != nullptr)
        {
            model->SetArmature(m_oldID[i]);

            m_workspace->PushLongTask(new TriangulateCurveLongTask(model));
        }
    }

    return true;
}