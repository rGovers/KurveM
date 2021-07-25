#pragma once

#include "Actions/Action.h"

class Animation;
class Workspace;

class DeleteAnimationAction : public Action
{
private:
    Workspace* m_workspace;

    bool       m_own;
    Animation* m_animation;

protected:

public:
    DeleteAnimationAction(Workspace* a_workspace, Animation* a_animation);
    ~DeleteAnimationAction();

    virtual e_ActionType GetActionType(); 

    virtual bool Redo();
    virtual bool Execute();
    virtual bool Revert();
};