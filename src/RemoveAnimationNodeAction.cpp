#include "Actions/RemoveAnimationNodeAction.h"

RemoveAnimationNodeAction::RemoveAnimationNodeAction(Animation* a_animation, Object* a_object, const AnimationNode& a_node)
{
    m_object = a_object;
    m_animation = a_animation;

    m_node = a_node;
}
RemoveAnimationNodeAction::~RemoveAnimationNodeAction()
{

}

e_ActionType RemoveAnimationNodeAction::GetActionType() const
{
    return ActionType_RemoveAnimationNode;
}

bool RemoveAnimationNodeAction::Redo()
{
    return Execute();
}
bool RemoveAnimationNodeAction::Execute()
{
    m_animation->RemoveNode(m_object, m_node);

    return true;
}
bool RemoveAnimationNodeAction::Revert()
{
    m_animation->AddNode(m_object, m_node);

    return true;
}