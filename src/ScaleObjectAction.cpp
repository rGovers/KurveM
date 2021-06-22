#include "Actions/ScaleObjectAction.h"

#include "Object.h"
#include "Transform.h"

ScaleObjectAction::ScaleObjectAction(const glm::vec3& a_newScale, Object* a_object)
{
    m_object = a_object;
    
    m_newScale = a_newScale;
    m_oldScale = m_object->GetTransform()->Scale();
}
ScaleObjectAction::~ScaleObjectAction()
{

}

e_ActionType ScaleObjectAction::GetActionType()
{
    return ActionType_ScaleObject;
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
    m_object->GetTransform()->Scale() = m_newScale;

    return true;
}
bool ScaleObjectAction::Revert()
{
    m_object->GetTransform()->Scale() = m_oldScale;

    return true;
}