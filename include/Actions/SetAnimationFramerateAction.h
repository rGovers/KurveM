#pragma once

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

#include "Actions/Action.h"

class Animation;

class SetAnimationFramerateAction : public Action
{
private:
    int        m_framerate;
    int        m_oldFramerate;

    Animation* m_animation;

protected:

public:
    SetAnimationFramerateAction(Animation* a_animation, int a_framerate);
    ~SetAnimationFramerateAction();

    virtual e_ActionType GetActionType();

    inline void SetFramerate(int a_value)
    {
        m_framerate = a_value;
    }

    virtual bool Redo();
    virtual bool Execute();
    virtual bool Revert();
};