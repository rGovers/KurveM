#pragma once

#include "Actions/Action.h"

class Animation;
class Workspace;

class CreateAnimationAction : public Action
{
private:
    Workspace* m_workspace;

    bool       m_own;
    Animation* m_animation;

    char*      m_name;

protected:

public:
    CreateAnimationAction(Workspace* a_workspace, const char* a_name);
    virtual ~CreateAnimationAction();

    virtual e_ActionType GetActionType() const; 

    virtual bool Redo();
    virtual bool Execute();
    virtual bool Revert();
};