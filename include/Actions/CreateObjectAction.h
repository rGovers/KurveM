#pragma once

#include "Actions/Action.h"

class Object;
class Workspace;

enum e_CreateObjectType
{
    CreateObjectType_Empty,
    CreateObjectType_SphereCurve,
    CreateObjectType_CubeCurve
};

class CreateObjectAction : public Action
{
private:
    Workspace*         m_workspace;
     
    Object*            m_parentObject;
    Object*            m_object;

    e_CreateObjectType m_objectType;

protected:

public:
    CreateObjectAction(Workspace* a_workspace, Object* a_parent, e_CreateObjectType a_type = CreateObjectType_Empty);
    ~CreateObjectAction();

    virtual bool Redo();
    virtual bool Execute();
    virtual bool Revert();
};