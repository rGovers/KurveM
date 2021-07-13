#pragma once

#include "Actions/Action.h"

class Object;
class Workspace;

enum e_CreateObjectType
{
    CreateObjectType_Empty,
    CreateObjectType_TriangleCurve,
    CreateObjectType_PlaneCurve,
    CreateObjectType_SphereCurve,
    CreateObjectType_CubeCurve,
    CreateObjectType_ReferenceImage
};

class CreateObjectAction : public Action
{
private:
    Workspace*         m_workspace;
     
    Object*            m_parentObject;
    Object*            m_object;

    const char*        m_texPath;

    e_CreateObjectType m_objectType;

protected:

public:
    CreateObjectAction(Workspace* a_workspace, Object* a_parent, e_CreateObjectType a_type = CreateObjectType_Empty);
    CreateObjectAction(Workspace* a_workspace, Object* a_parent, const char* a_texPath);
    ~CreateObjectAction();

    virtual e_ActionType GetActionType();

    virtual bool Redo();
    virtual bool Execute();
    virtual bool Revert();
};