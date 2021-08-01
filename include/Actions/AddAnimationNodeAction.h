#pragma once

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

#include "Actions/Action.h"

#include "Animation.h"

class Object;

class AddAnimationNodeAction : public Action
{
private:
    float         m_time;

    AnimationNode m_node;

    Object*       m_object;
    Animation*    m_animation;

protected:

public:
    AddAnimationNodeAction(Animation* a_animation, Object* a_object, const AnimationNode& a_node);
    ~AddAnimationNodeAction();

    virtual e_ActionType GetActionType();

    virtual bool Redo();
    virtual bool Execute();
    virtual bool Revert();
};