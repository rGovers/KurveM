#include "Actions/RotateObjectAction.h"

#include "Object.h"
#include "Transform.h"

RotateObjectAction::RotateObjectAction(const glm::quat& a_newRotation, Object* a_object) :
    RotateObjectAction(a_newRotation, &a_object, 1)
{

}
RotateObjectAction::RotateObjectAction(const glm::quat& a_newRotation, Object** a_objects, unsigned int a_objectCount)
{
    m_objectCount = a_objectCount;

    m_newRotation = a_newRotation;

    m_objects = new Object*[m_objectCount];
    m_oldRotation = new glm::quat[m_objectCount];

    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        m_objects[i] = a_objects[i];
        m_oldRotation[i] = m_objects[i]->GetTransform()->Quaternion();
    }
}
RotateObjectAction::~RotateObjectAction()
{
    delete[] m_objects;
    delete[] m_oldRotation;
}

e_ActionType RotateObjectAction::GetActionType()
{
    return ActionType_RotateObject;
}

bool RotateObjectAction::Redo()
{
    return Execute();
}
bool RotateObjectAction::Execute()
{
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        m_objects[i]->GetTransform()->Quaternion() = m_newRotation;
    }

    return true;
}
bool RotateObjectAction::Revert()
{
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        m_objects[i]->GetTransform()->Quaternion() = m_oldRotation[i];
    }

    return true;
}