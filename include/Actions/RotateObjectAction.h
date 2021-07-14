#pragma once

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

#include <glm/gtc/quaternion.hpp>

#include "Actions/Action.h"

class Object;

class RotateObjectAction : public Action
{
private:
    glm::quat*   m_oldRotation;
    glm::quat    m_newRotation;

    unsigned int m_objectCount;
    Object**     m_objects;
protected:

public:
    RotateObjectAction(const glm::quat& a_newRotation, Object* a_object);
    RotateObjectAction(const glm::quat& a_newRotation, Object** a_objects, unsigned int a_objectCount);
    virtual ~RotateObjectAction();

    virtual e_ActionType GetActionType();

    inline void SetRotation(const glm::quat& a_newRotation)
    {
        m_newRotation = a_newRotation;
    }

    virtual bool Redo();
    virtual bool Execute();
    virtual bool Revert();
};