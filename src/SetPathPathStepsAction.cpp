#include "Actions/SetPathPathStepsAction.h"

#include "LongTasks/TriangulatePathLongTask.h"
#include "Object.h"
#include "PathModel.h"
#include "Workspace.h"

SetPathPathStepsAction::SetPathPathStepsAction(Workspace* a_workspace, Object* const * a_objects, unsigned int a_objectCount, int a_steps)
{
    m_workspace = a_workspace;

    m_objectCount = a_objectCount;
    m_newSteps = a_steps;

    m_objects = new Object*[m_objectCount];
    m_oldSteps = new int[m_objectCount];

    for (unsigned int i = 0; i < a_objectCount; ++i)
    {
        m_objects[i] = a_objects[i];
        const PathModel* model = m_objects[i]->GetPathModel();
        m_oldSteps[i] = model->GetPathSteps();
    }
}
SetPathPathStepsAction::~SetPathPathStepsAction()
{
    delete[] m_oldSteps;

    delete[] m_objects;
}

e_ActionType SetPathPathStepsAction::GetActionType() const
{
    return ActionType_SetPathPathSteps;
}

bool SetPathPathStepsAction::Redo()
{
    return Execute();
}
bool SetPathPathStepsAction::Execute()
{
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        PathModel* model = m_objects[i]->GetPathModel();
        if (model != nullptr)
        {
            model->SetPathSteps(m_newSteps);

            m_workspace->PushLongTask(new TriangulatePathLongTask(model));
        }
    }

    return true;
}
bool SetPathPathStepsAction::Revert()
{
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        PathModel* model = m_objects[i]->GetPathModel();
        if (model != nullptr)
        {
            model->SetPathSteps(m_oldSteps[i]);

            m_workspace->PushLongTask(new TriangulatePathLongTask(model));
        }
    }

    return true;
}