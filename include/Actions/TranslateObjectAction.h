#pragma once

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

#include "Actions/Action.h"

class Object;

class TranslateObjectAction : public Action
{
private:
    glm::vec3 m_oldPos;
    glm::vec3 m_newPos;

    Object*   m_object;

protected:

public:
    TranslateObjectAction(const glm::vec3& a_newPos, Object* a_object);
    ~TranslateObjectAction();

    virtual e_ActionType GetActionType();

    void SetTranslation(const glm::vec3& a_newPos);

    virtual bool Redo();
    virtual bool Execute();
    virtual bool Revert();
};