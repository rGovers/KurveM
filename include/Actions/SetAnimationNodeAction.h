#pragma once

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

#include "Actions/Action.h"

#include "Animation.h"

class Object;

class SetAnimationNodeAction : public Action
{
private:
    AnimationNode m_node;
    AnimationNode m_oldNode;

    const Object* m_object;
    Animation*    m_animation;

protected:

public:
    SetAnimationNodeAction(Animation* a_animation, const Object* a_object, const AnimationNode& a_node);
    virtual ~SetAnimationNodeAction();

    virtual e_ActionType GetActionType() const;

    inline void SetNode(const AnimationNode& a_node)
    {
        m_node = a_node;
    }

    virtual void SetData(void* a_data);

    virtual bool Redo();
    virtual bool Execute();
    virtual bool Revert();
};