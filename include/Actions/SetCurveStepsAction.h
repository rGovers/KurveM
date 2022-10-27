#pragma once

#include "Actions/Action.h"

class Object;
class Workspace;

class SetCurveStepsAction : public Action
{
private:
    Workspace*   m_workspace;

    int*         m_oldSteps;
    int          m_newSteps; 

    unsigned int m_objectCount;

    Object**     m_objects;

protected:

public:
    SetCurveStepsAction(Workspace* a_workspace, Object** a_objects, unsigned int a_objectCount, int a_steps);
    virtual ~SetCurveStepsAction();

    virtual e_ActionType GetActionType() const;

    inline void SetSteps(int a_steps)
    {
        m_newSteps = a_steps;
    }
    virtual void SetData(void* a_data);

    virtual bool Redo();
    virtual bool Execute();
    virtual bool Revert();
};