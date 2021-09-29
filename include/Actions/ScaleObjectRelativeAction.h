#pragma once

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

#include "Actions/Action.h"

class Object;

class ScaleObjectRelativeAction : public Action
{
private:
    unsigned int m_objectCount;
    Object**     m_objects;

    glm::vec3*   m_oldScale;

    glm::vec3    m_startPos;
    glm::vec3    m_endPos;

    glm::vec3    m_axis;

protected:

public:
    ScaleObjectRelativeAction(const glm::vec3& a_startPos, const glm::vec3& a_axis, Object* a_object);
    ScaleObjectRelativeAction(const glm::vec3& a_startPos, const glm::vec3& a_axis, Object* const* a_objects, unsigned int a_objectCount);
    virtual ~ScaleObjectRelativeAction();

    virtual e_ActionType GetActionType();

    inline void SetScale(const glm::vec3& a_endPos)
    {
        m_endPos = a_endPos;
    }

    virtual bool Redo();
    virtual bool Execute();
    virtual bool Revert();
};