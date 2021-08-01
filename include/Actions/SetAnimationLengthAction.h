#pragma once

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

#include "Actions/Action.h"

class Animation;

class SetAnimationLengthAction : public Action
{
private:
    float      m_time;
    float      m_oldTime;

    Animation* m_animation;

protected:

public:
    SetAnimationLengthAction(Animation* a_animation, float a_length);
    ~SetAnimationLengthAction();

    virtual e_ActionType GetActionType();

    inline void SetLength(float a_value)
    {
        m_time = a_value;
    }

    virtual bool Redo();
    virtual bool Execute();
    virtual bool Revert();
};