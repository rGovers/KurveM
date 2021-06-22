#include "Actions/TranslateObjectAction.h"

#include "Object.h"
#include "Transform.h"

TranslateObjectAction::TranslateObjectAction(const glm::vec3& a_newPos, Object* a_object)
{
    m_object = a_object;
    
    m_newPos = a_newPos;
    m_oldPos = m_object->GetTransform()->Translation();
}
TranslateObjectAction::~TranslateObjectAction()
{

}

e_ActionType TranslateObjectAction::GetActionType()
{
    return ActionType_TranslateObject;
}

void TranslateObjectAction::SetTranslation(const glm::vec3& a_newPos)
{
    m_newPos = a_newPos;
}

bool TranslateObjectAction::Redo()
{
    return Execute();
}
bool TranslateObjectAction::Execute()
{
    m_object->GetTransform()->Translation() = m_newPos;

    return true;
}
bool TranslateObjectAction::Revert()
{
    m_object->GetTransform()->Translation() = m_oldPos;

    return true;
}