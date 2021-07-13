#include "Actions/TranslateObjectRelativeAction.h"

#include "Object.h"
#include "Transform.h"

TranslateObjectRelativeAction::TranslateObjectRelativeAction(const glm::vec3& a_curPos, const glm::vec3& a_axis, Object* a_object) : 
    TranslateObjectRelativeAction(a_curPos, a_axis, &a_object, 1)
{

}
TranslateObjectRelativeAction::TranslateObjectRelativeAction(const glm::vec3& a_curPos, const glm::vec3& a_axis, Object** a_objects, unsigned int a_objectCount)
{
    m_objectCount = a_objectCount;

    m_objects = new Object*[m_objectCount];
    m_oldPos = new glm::vec3[m_objectCount];

    m_startPos = a_curPos;
    m_endPos = a_curPos;

    m_axis = a_axis;

    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        m_objects[i] = a_objects[i];
        m_oldPos[i] = m_objects[i]->GetTransform()->Translation();
    }
}
TranslateObjectRelativeAction::~TranslateObjectRelativeAction()
{
    delete[] m_objects;
    delete[] m_oldPos;
}

e_ActionType TranslateObjectRelativeAction::GetActionType()
{
    return ActionType_TranslateObjectRelative;
}

bool TranslateObjectRelativeAction::Redo()
{
    return Execute();
}
bool TranslateObjectRelativeAction::Execute()
{
    const glm::vec3 endAxis = m_endPos - m_startPos;
        
    const float len = glm::length(endAxis);

    if (len != 0)
    {
        const glm::vec3 scaledAxis = m_axis * len;

        const float scale = glm::dot(scaledAxis, endAxis); 

        for (unsigned int i = 0; i < m_objectCount; ++i)
        {
            const glm::vec3 diff = m_oldPos[i] - m_startPos;

            m_objects[i]->GetTransform()->Translation() = (m_startPos + (m_axis * scale)) + diff;
        }
    }

    return true;
}
bool TranslateObjectRelativeAction::Revert()
{
    for (unsigned int i = 0; i < m_objectCount; ++i)
    {
        m_objects[i]->GetTransform()->Translation() = m_oldPos[i];
    }

    return true;
}