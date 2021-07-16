#include "Actions/SetCurveSmartStepAction.h"

#include "CurveModel.h"
#include "LongTasks/TriangulateCurveLongTask.h"
#include "Object.h"
#include "Workspace.h"

SetCurveSmartStepAction::SetCurveSmartStepAction(Workspace* a_workspace, Object** a_objects, unsigned int a_objectCount, bool a_value)
{
    m_workspace = a_workspace;

    m_objectCount = a_objectCount;

    m_newValue = a_value;

    m_oldValues = std::vector<bool>(m_objectCount);
    m_objects = new Object*[m_objectCount];

    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        m_objects[i] = a_objects[i];

        const CurveModel* model = m_objects[i]->GetCurveModel();
        if (model != nullptr)
        {
            m_oldValues[i] = model->GetSteps();
        }
    }
}
SetCurveSmartStepAction::~SetCurveSmartStepAction()
{
    delete[] m_objects;
}

e_ActionType SetCurveSmartStepAction::GetActionType() 
{
    return ActionType_SetCurveSmartStepAction;
}

bool SetCurveSmartStepAction::Redo()
{
    return Execute();
}

bool SetCurveSmartStepAction::Execute()
{
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        CurveModel* model = m_objects[i]->GetCurveModel();
        if (model != nullptr)
        {
            model->SetStepAdjust(m_newValue);

            m_workspace->PushLongTask(new TriangulateCurveLongTask(model));
        }
    }

    return true;
}
bool SetCurveSmartStepAction::Revert()
{
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        CurveModel* model = m_objects[i]->GetCurveModel();
        if (model != nullptr)
        {
            model->SetStepAdjust(m_oldValues[i]);

            m_workspace->PushLongTask(new TriangulateCurveLongTask(model));
        }
    }

    return true;
}