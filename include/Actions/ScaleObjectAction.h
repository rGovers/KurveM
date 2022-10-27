#pragma once

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

#include "Actions/Action.h"

class Object;

class ScaleObjectAction : public Action
{
private:
    glm::vec3*   m_oldScale;
    glm::vec3    m_newScale;

    unsigned int m_objectCount;
    Object**     m_objects;

protected:

public:
    ScaleObjectAction(const glm::vec3& a_newScale, Object* a_object);
    ScaleObjectAction(const glm::vec3& a_newScale, Object** a_objects, unsigned int a_objectCount);
    ~ScaleObjectAction();

    virtual e_ActionType GetActionType() const;

    void SetScale(const glm::vec3& a_newScale);

    virtual void SetData(void* a_data);

    virtual bool Redo();
    virtual bool Execute();
    virtual bool Revert();
};