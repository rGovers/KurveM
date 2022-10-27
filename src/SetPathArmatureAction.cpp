#include "Actions/SetPathArmatureAction.h"

#include "Object.h"
#include "PathModel.h"

SetPathArmatureAction::SetPathArmatureAction(Workspace* a_workspace, Object* const* a_objects, unsigned int a_objectCount, long long a_id)
{
    m_workspace = a_workspace;

    m_newID = a_id;

    m_objectCount = a_objectCount;

    m_objects = new Object*[m_objectCount];
    m_oldID = new long long[m_objectCount];
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        Object* obj = a_objects[i];
        m_objects[i] = obj;

        const PathModel* model = obj->GetPathModel();
        if (model != nullptr)
        {
            m_oldID[i] = model->GetArmatureID();
        }
    }
}
SetPathArmatureAction::~SetPathArmatureAction()
{
    delete[] m_oldID;
    delete[] m_objects;
}

e_ActionType SetPathArmatureAction::GetActionType() const
{
    return ActionType_SetPathArmature;
}

bool SetPathArmatureAction::Redo()
{
    return Execute();
}
bool SetPathArmatureAction::Execute()
{
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        PathModel* model = m_objects[i]->GetPathModel();
        if (model != nullptr)
        {
            model->SetArmature(m_newID);
        }
    }

    return true;
}
bool SetPathArmatureAction::Revert()
{
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        PathModel* model = m_objects[i]->GetPathModel();
        if (model != nullptr)
        {
            model->SetArmature(m_oldID[i]);
        }
    }

    return true;
}