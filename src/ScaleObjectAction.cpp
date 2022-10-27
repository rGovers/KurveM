#include "Actions/ScaleObjectAction.h"

#include "Object.h"
#include "Transform.h"

ScaleObjectAction::ScaleObjectAction(const glm::vec3& a_newScale, Object* a_object) :
    ScaleObjectAction(a_newScale, &a_object, 1)
{

}
ScaleObjectAction::ScaleObjectAction(const glm::vec3& a_newScale, Object** a_objects, unsigned int a_objectCount)
{
    m_objectCount = a_objectCount;

    m_objects = new Object*[m_objectCount];
    m_oldScale = new glm::vec3[m_objectCount];

    m_newScale = a_newScale;

    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        m_objects[i] = a_objects[i];
        m_oldScale[i] = m_objects[i]->GetTransform()->Scale();
    }
}
ScaleObjectAction::~ScaleObjectAction()
{
    delete[] m_objects;
    delete[] m_oldScale;
}

e_ActionType ScaleObjectAction::GetActionType() const
{
    return ActionType_ScaleObject;
}

void ScaleObjectAction::SetData(void* a_data)
{
    m_newScale = *(glm::vec3*)a_data;
}

void ScaleObjectAction::SetScale(const glm::vec3& a_newScale)
{
    m_newScale = a_newScale;
}

bool ScaleObjectAction::Redo()
{
    return Execute();
}
bool ScaleObjectAction::Execute()
{
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        m_objects[i]->GetTransform()->Scale() = m_newScale;
    }

    return true;
}
bool ScaleObjectAction::Revert()
{
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        m_objects[i]->GetTransform()->Scale() = m_oldScale[i];
    }

    return true;
}