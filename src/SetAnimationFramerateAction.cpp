#include "Actions/SetAnimationFramerateAction.h"

#include "Animation.h"

SetAnimationFramerateAction::SetAnimationFramerateAction(Animation* a_animation, int a_framerate)
{
    m_animation = a_animation;

    m_framerate = a_framerate;
    m_oldFramerate = m_animation->GetReferenceFramerate();
}
SetAnimationFramerateAction::~SetAnimationFramerateAction()
{
    
}

e_ActionType SetAnimationFramerateAction::GetActionType() const
{
    return ActionType_SetAnimationFramerate;
}

bool SetAnimationFramerateAction::Redo()
{
    return Execute();
}
bool SetAnimationFramerateAction::Execute()
{
    m_animation->SetReferenceFramerate(m_framerate);

    return true;
}
bool SetAnimationFramerateAction::Revert()
{
    m_animation->SetReferenceFramerate(m_oldFramerate);

    return true;
}