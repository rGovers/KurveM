#pragma once

#include "Actions/Action.h"

#include <vector>

class Object;
class Workspace;

class SetCurveSmartStepAction : public Action
{
private:
    Workspace*        m_workspace;

    std::vector<bool> m_oldValues;
    bool              m_newValue; 

    unsigned int      m_objectCount;

    Object**          m_objects;

protected:

public:
    SetCurveSmartStepAction(Workspace* a_workspace, Object** a_objects, unsigned int a_objectCount, bool a_value);
    ~SetCurveSmartStepAction();

    virtual e_ActionType GetActionType();

    inline void SetValue(bool a_value)
    {
        m_newValue = a_value;
    }

    virtual bool Redo();
    virtual bool Execute();
    virtual bool Revert();
};