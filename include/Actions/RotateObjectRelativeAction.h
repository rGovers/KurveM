#pragma once

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

#include "Actions/Action.h"

#include <glm/gtc/quaternion.hpp>

class Object;

class RotateObjectRelativeAction : public Action
{
private:
    unsigned int m_objectCount;
    Object**     m_objects;

    glm::vec3    m_centre;

    glm::vec3*   m_oldPositions;
    glm::quat*   m_oldRotations;

    glm::vec3    m_startPos;
    glm::vec3    m_endPos;

    glm::vec3    m_axis;

protected:

public:
    RotateObjectRelativeAction(const glm::vec3& a_startPos, const glm::vec3& a_axis, Object* a_object);
    RotateObjectRelativeAction(const glm::vec3& a_startPos, const glm::vec3& a_axis, Object* const* a_objects, unsigned int a_objectCount);
    virtual ~RotateObjectRelativeAction();

    virtual e_ActionType GetActionType();

    inline void SetRotation(const glm::vec3& a_endPos)
    {
        m_endPos = a_endPos;
    }

    virtual bool Redo();
    virtual bool Execute();
    virtual bool Revert();
};