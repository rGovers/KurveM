#include "Actions/RotateObjectRelativeAction.h"

#include <glm/gtx/quaternion.hpp>

#include "Object.h"
#include "Transform.h"

RotateObjectRelativeAction::RotateObjectRelativeAction(const glm::vec3& a_startPos, const glm::vec3& a_axis, Object* a_object) :
    RotateObjectRelativeAction(a_startPos, a_axis, &a_object, 1U)
{
}
RotateObjectRelativeAction::RotateObjectRelativeAction(const glm::vec3& a_startPos, const glm::vec3& a_axis, Object* const* a_object, unsigned int a_objectCount)
{
    m_objectCount = a_objectCount;

    m_axis = a_axis;

    m_objects = new Object*[m_objectCount];

    m_oldRotations = new glm::quat[m_objectCount];
    m_oldPositions = new glm::vec3[m_objectCount];

    m_centre = glm::vec3(0);

    m_startPos = a_startPos;
    m_endPos = a_startPos;

    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        Object* obj = a_object[i];

        m_objects[i] = obj;
        
        const Transform* transform = obj->GetTransform(); 
        
        const glm::vec3 pos = transform->Translation();
        
        m_oldRotations[i] = transform->Quaternion();
        m_oldPositions[i] = pos;

        m_centre += pos;
    }

    m_centre /= m_objectCount;
}
RotateObjectRelativeAction::~RotateObjectRelativeAction()
{
    delete[] m_oldRotations;
    delete[] m_oldPositions;

    delete[] m_objects;
}

e_ActionType RotateObjectRelativeAction::GetActionType() 
{
    return ActionType_RotateObjectRelative;
}

bool RotateObjectRelativeAction::Redo()
{
    return Execute();
}
bool RotateObjectRelativeAction::Execute()
{
    const glm::vec3 endAxis = m_endPos - m_startPos;

    const float len = glm::length(endAxis);

    if (len > 0)
    {
        const glm::vec3 inv = glm::vec3(1) - m_axis;

        const glm::vec3 scaledAxis = inv * len;
        
        const float scale = glm::dot(scaledAxis, endAxis) * 10;

        const glm::quat q = glm::angleAxis(scale, m_axis);

        for (unsigned int i = 0; i < m_objectCount; ++i)
        {
            const glm::vec3 diff = m_oldPositions[i] - m_centre;

            Transform* transform = m_objects[i]->GetTransform();

            // transform->Translation() = m_centre + (q * diff);
            transform->Quaternion() = q * m_oldRotations[i];
        }
    }

    return true;
}
bool RotateObjectRelativeAction::Revert()
{
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        Transform* transform = m_objects[i]->GetTransform();

        transform->Quaternion() = m_oldRotations[i];
        transform->Translation() = m_oldPositions[i];
    }

    return true;
}