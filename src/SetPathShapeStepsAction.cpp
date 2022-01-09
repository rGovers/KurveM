#include "Actions/SetPathShapeStepsAction.h"

#include "LongTasks/TriangulatePathLongTask.h"
#include "Object.h"
#include "PathModel.h"
#include "Workspace.h"

SetPathShapeStepsAction::SetPathShapeStepsAction(Workspace* a_workspace, Object* const * a_objects, unsigned int a_objectCount, int a_steps)
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
        m_oldSteps[i] = model->GetShapeSteps();
    }
}
SetPathShapeStepsAction::~SetPathShapeStepsAction()
{
    delete[] m_oldSteps;

    delete[] m_objects;
}

e_ActionType SetPathShapeStepsAction::GetActionType()
{
    return ActionType_SetPathShapeSteps;
}

bool SetPathShapeStepsAction::Redo()
{
    return Execute();
}
bool SetPathShapeStepsAction::Execute()
{
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        PathModel* model = m_objects[i]->GetPathModel();
        if (model != nullptr)
        {
            model->SetShapeSteps(m_newSteps);

            m_workspace->PushLongTask(new TriangulatePathLongTask(model));
        }
    }

    return true;
}
bool SetPathShapeStepsAction::Revert()
{
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        PathModel* model = m_objects[i]->GetPathModel();
        if (model != nullptr)
        {
            model->SetShapeSteps(m_oldSteps[i]);

            m_workspace->PushLongTask(new TriangulatePathLongTask(model));
        }
    }

    return true;
}