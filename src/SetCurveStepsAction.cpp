#include "Actions/SetCurveStepsAction.h"

#include "CurveModel.h"
#include "LongTasks/TriangulateCurveLongTask.h"
#include "Object.h"
#include "Workspace.h"

SetCurveStepsAction::SetCurveStepsAction(Workspace* a_workspace, Object** a_objects, unsigned int a_objectCount, int a_steps)
{
    m_workspace = a_workspace;

    m_objectCount = a_objectCount;

    m_newSteps = a_steps;

    m_oldSteps = new int[m_objectCount];
    m_objects = new Object*[m_objectCount];

    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        m_objects[i] = a_objects[i];

        const CurveModel* model = m_objects[i]->GetCurveModel();
        if (model != nullptr)
        {
            m_oldSteps[i] = model->GetSteps();
        }
    }
}
SetCurveStepsAction::~SetCurveStepsAction()
{
    delete[] m_oldSteps;
    delete[] m_objects;
}

e_ActionType SetCurveStepsAction::GetActionType() const
{
    return ActionType_SetCurveSteps;
}

void SetCurveStepsAction::SetData(void* a_data)
{
    m_newSteps = *(int*)a_data;
}

bool SetCurveStepsAction::Redo()
{
    return Execute();
}

bool SetCurveStepsAction::Execute()
{
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        CurveModel* model = m_objects[i]->GetCurveModel();
        if (model != nullptr)
        {
            model->SetSteps(m_newSteps);

            m_workspace->PushLongTask(new TriangulateCurveLongTask(model));
        }
    }

    return true;
}
bool SetCurveStepsAction::Revert()
{
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        CurveModel* model = m_objects[i]->GetCurveModel();
        if (model != nullptr)
        {
            model->SetSteps(m_oldSteps[i]);

            m_workspace->PushLongTask(new TriangulateCurveLongTask(model));
        }
    }

    return true;
}