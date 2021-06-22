#pragma once

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

#include "Actions/Action.h"

class Object;

class ScaleObjectAction : public Action
{
private:
    glm::vec3 m_oldScale;
    glm::vec3 m_newScale;

    Object*   m_object;

protected:

public:
    ScaleObjectAction(const glm::vec3& a_newScale, Object* a_object);
    ~ScaleObjectAction();

    virtual e_ActionType GetActionType();

    void SetScale(const glm::vec3& a_newScale);

    virtual bool Redo();
    virtual bool Execute();
    virtual bool Revert();
};