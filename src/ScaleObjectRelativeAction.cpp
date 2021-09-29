#include "Actions/ScaleObjectRelativeAction.h"

#include "Object.h"
#include "Transform.h"

ScaleObjectRelativeAction::ScaleObjectRelativeAction(const glm::vec3& a_startPos, const glm::vec3& a_axis, Object* a_object) :
    ScaleObjectRelativeAction(a_startPos, a_axis, &a_object, 1)
{

}
ScaleObjectRelativeAction::ScaleObjectRelativeAction(const glm::vec3& a_startPos, const glm::vec3& a_axis, Object* const* a_objects, unsigned int a_objectCount)
{
    m_startPos = a_startPos;
    m_endPos = a_startPos;

    m_axis = a_axis;

    m_objectCount = a_objectCount;

    m_objects = new Object*[m_objectCount];
    m_oldScale = new glm::vec3[m_objectCount];

    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        Object* obj = a_objects[i];

        m_objects[i] = obj;
        m_oldScale[i] = obj->GetTransform()->Scale();
    }
}
ScaleObjectRelativeAction::~ScaleObjectRelativeAction()
{
    delete[] m_objects;
    delete[] m_oldScale;
}

e_ActionType ScaleObjectRelativeAction::GetActionType()
{
    return ActionType_ScaleObjectRelative;
}

bool ScaleObjectRelativeAction::Redo()
{
    return Execute();
}
bool ScaleObjectRelativeAction::Execute()
{
    const glm::vec3 endAxis = m_endPos - m_startPos;
        
    const float len = glm::length(endAxis);

    if (len > 0)
    {
        const glm::vec3 scaledAxis = m_axis * len;

        const float scale = glm::dot(scaledAxis, endAxis); 

        const glm::vec3 scaleAxis = m_axis * scale;

        for (unsigned int i = 0; i < m_objectCount; ++i)
        {
            m_objects[i]->GetTransform()->Scale() = m_oldScale[i] + scaleAxis;
        }
    }

    return true;
}
bool ScaleObjectRelativeAction::Revert()
{
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        m_objects[i]->GetTransform()->Scale() = m_oldScale[i];
    }

    return true;
}