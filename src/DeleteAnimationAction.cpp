#include "Actions/DeleteAnimationAction.h"

#include "Workspace.h"

DeleteAnimationAction::DeleteAnimationAction(Workspace* a_workspace, Animation* a_animation)
{
    m_workspace = a_workspace;

    m_animation = a_animation;

    m_own = false;
}
DeleteAnimationAction::~DeleteAnimationAction()
{
    if (m_own)
    {
        delete m_animation;
    }
}

e_ActionType DeleteAnimationAction::GetActionType()
{
    return ActionType_DeleteAnimation;
}

bool DeleteAnimationAction::Redo()
{
    return Execute();
}
bool DeleteAnimationAction::Execute()
{
    m_workspace->RemoveAnimation(m_animation);
    m_own = true;

    return true;
}
bool DeleteAnimationAction::Revert()
{
    m_workspace->AddAnimation(m_animation);
    m_own = false;

    return true;
}