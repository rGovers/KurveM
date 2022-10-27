#include "Actions/TranslateObjectAction.h"

#include "Object.h"
#include "Transform.h"

TranslateObjectAction::TranslateObjectAction(const glm::vec3& a_newPos, Object* a_object) : 
    TranslateObjectAction(a_newPos, &a_object, 1)
{

}
TranslateObjectAction::TranslateObjectAction(const glm::vec3& a_newPos, Object** a_objects, unsigned int a_objectCount)
{
    m_objectCount = a_objectCount;

    m_objects = new Object*[m_objectCount];
    m_oldPos = new glm::vec3[m_objectCount];

    m_newPos = a_newPos;

    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        m_objects[i] = a_objects[i];
        m_oldPos[i] = m_objects[i]->GetTransform()->Translation();
    }
}
TranslateObjectAction::~TranslateObjectAction()
{
    delete[] m_objects;
    delete[] m_oldPos;
}

e_ActionType TranslateObjectAction::GetActionType() const
{
    return ActionType_TranslateObject;
}

void TranslateObjectAction::SetData(void* a_data)
{
    m_newPos = *(glm::vec3*)a_data;
}

bool TranslateObjectAction::Redo()
{
    return Execute();
}
bool TranslateObjectAction::Execute()
{
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        m_objects[i]->GetTransform()->Translation() = m_newPos;
    }

    return true;
}
bool TranslateObjectAction::Revert()
{
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        m_objects[i]->GetTransform()->Translation() = m_oldPos[i];
    }

    return true;
}