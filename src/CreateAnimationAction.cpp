#include "Actions/CreateAnimationAction.h"

#include <string.h>

#include "Animation.h"
#include "Workspace.h"

CreateAnimationAction::CreateAnimationAction(Workspace* a_workspace, const char* a_name)
{
    m_workspace = a_workspace;

    const int length = strlen(a_name) + 1;

    m_name = new char[length];

    for (int i = 0; i < length; ++i)
    {
        m_name[i] = a_name[i];
    }

    m_own = false;
    m_animation = nullptr;
}
CreateAnimationAction::~CreateAnimationAction()
{
    if (m_own && m_animation != nullptr)
    {
        delete m_animation;
    }

    delete[] m_name;
}

e_ActionType CreateAnimationAction::GetActionType() const
{
    return ActionType_CreateAnimation;
}

bool CreateAnimationAction::Redo()
{
    return Execute();
}
bool CreateAnimationAction::Execute()
{
    m_own = false;

    if (m_animation == nullptr)
    {
        m_animation = new Animation(m_name);
    }

    m_workspace->AddAnimation(m_animation);
    m_workspace->SetCurrentAnimation(m_animation);

    return true;
}
bool CreateAnimationAction::Revert()
{
    m_own = true;

    if (m_animation != nullptr)
    {
        m_workspace->RemoveAnimation(m_animation);
    }

    return true;
}