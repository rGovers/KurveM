#pragma once

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

#include "Actions/Action.h"

class Object;

class TranslateObjectRelativeAction : public Action
{
private:
    glm::vec3*   m_oldPos;

    unsigned int m_objectCount;
    Object**     m_objects;

    glm::vec3    m_axis;

    glm::vec3    m_startPos;
    glm::vec3    m_endPos;
protected:

public:
    TranslateObjectRelativeAction(const glm::vec3& a_startPos, const glm::vec3& a_axis, Object* a_object);
    TranslateObjectRelativeAction(const glm::vec3& a_startPos, const glm::vec3& a_axis, Object** a_objects, unsigned int a_objectCount);
    ~TranslateObjectRelativeAction();

    virtual e_ActionType GetActionType();

    inline void SetTranslation(const glm::vec3& a_endPos)
    {
        m_endPos = a_endPos;
    }

    virtual bool Redo();
    virtual bool Execute();
    virtual bool Revert();
};