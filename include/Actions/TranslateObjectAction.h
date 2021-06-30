#pragma once

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

#include "Actions/Action.h"

class Object;

class TranslateObjectAction : public Action
{
private:
    glm::vec3*   m_oldPos;
    glm::vec3    m_newPos;

    unsigned int m_objectCount;
    Object**     m_objects;

protected:

public:
    TranslateObjectAction(const glm::vec3& a_newPos, Object* a_object);
    TranslateObjectAction(const glm::vec3& a_newPos, Object** a_objects, unsigned int a_objectCount);
    ~TranslateObjectAction();

    virtual e_ActionType GetActionType();

    inline void SetTranslation(const glm::vec3& a_newPos)
    {
        m_newPos = a_newPos;
    }

    virtual bool Redo();
    virtual bool Execute();
    virtual bool Revert();
};