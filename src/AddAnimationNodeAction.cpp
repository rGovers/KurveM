#include "Actions/AddAnimationNodeAction.h"

AddAnimationNodeAction::AddAnimationNodeAction(Animation* a_animation, Object* a_object, const AnimationNode& a_node)
{
    m_object = a_object;
    m_animation = a_animation;

    m_node = a_node;
}
AddAnimationNodeAction::~AddAnimationNodeAction()
{

}

e_ActionType AddAnimationNodeAction::GetActionType()
{
    return ActionType_AddAnimationNode;
}

bool AddAnimationNodeAction::Redo()
{
    return Execute();
}
bool AddAnimationNodeAction::Execute()
{
    m_animation->AddNode(m_object, m_node);

    return true;
}
bool AddAnimationNodeAction::Revert()
{
    m_animation->RemoveNode(m_object, m_node);

    return true;
}