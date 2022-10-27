#pragma once

#include "Actions/Action.h"

class Object;
class Workspace;

class SetVertexShaderAction : public Action
{
private:
    Workspace*   m_workspace;

    char**       m_oldShaders;
    char*        m_newShader;

    unsigned int m_objectCount;
    Object**     m_objects;

protected:

public:
    SetVertexShaderAction(Workspace* a_workspace, Object* const* a_objects, unsigned int a_objectCount, const char* a_newShader);
    virtual ~SetVertexShaderAction();

    virtual e_ActionType GetActionType() const;

    virtual bool Redo();
    virtual bool Execute();
    virtual bool Revert();
};