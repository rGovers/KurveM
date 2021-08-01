#include "Actions/SetAnimationNodeAction.h"

SetAnimationNodeAction::SetAnimationNodeAction(Animation* a_animation, Object* a_object, const AnimationNode& a_node)
{
    m_animation = a_animation;

    m_object = a_object;

    m_node = a_node;
    m_oldNode = m_animation->GetNode(m_object, a_node.Time);
}
SetAnimationNodeAction::~SetAnimationNodeAction()
{

}

e_ActionType SetAnimationNodeAction::GetActionType()
{
    return ActionType_SetAnimationNode;
}

bool SetAnimationNodeAction::Redo()
{
    return Execute();
}
bool SetAnimationNodeAction::Execute()
{
    if (m_oldNode.Time < 0)
    {
        return false;
    }

    m_animation->SetNode(m_object, m_node);

    return true;
}
bool SetAnimationNodeAction::Revert()
{
    if (m_oldNode.Time < 0)
    {
        return false;
    }

    m_animation->SetNode(m_object, m_oldNode);

    return true;
}