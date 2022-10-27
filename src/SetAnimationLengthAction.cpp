#include "Actions/SetAnimationLengthAction.h"

#include "Animation.h"

SetAnimationLengthAction::SetAnimationLengthAction(Animation* a_animation, float a_length)
{
    m_animation = a_animation;

    m_time = a_length;
    m_oldTime = m_animation->GetAnimationLength();
}
SetAnimationLengthAction::~SetAnimationLengthAction()
{

}

e_ActionType SetAnimationLengthAction::GetActionType() const
{
    return ActionType_SetAnimationLength;
}

bool SetAnimationLengthAction::Redo()
{
    return Execute();
}
bool SetAnimationLengthAction::Execute()
{
    m_animation->SetAnimationLength(m_time);

    return true;
}
bool SetAnimationLengthAction::Revert()
{
    m_animation->SetAnimationLength(m_oldTime);

    return true;
}