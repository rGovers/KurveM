#include "Actions/ExtrudeArmatureNodeAction.h"

#include "Object.h"
#include "Transform.h"

ExtrudeArmatureNodeAction::ExtrudeArmatureNodeAction(Editor* a_editor, Object** a_nodes, unsigned int a_nodeCount, const glm::vec3& a_startPos, const glm::vec3& a_axis)
{
    m_editor = a_editor;

    m_nodeCount = a_nodeCount;

    m_startPos = a_startPos;
    m_endPos = a_startPos;

    m_axis = a_axis;

    m_nodes = new Object*[m_nodeCount];

    for (unsigned int i = 0; i < m_nodeCount; ++i)
    {
        m_nodes[i] = a_nodes[i];
    }

    m_cNodes = nullptr;

    m_own = false;
}
ExtrudeArmatureNodeAction::~ExtrudeArmatureNodeAction()
{
    if (m_cNodes != nullptr)
    {
        if (m_own)
        {
            for (unsigned int i = 0; i < m_nodeCount; ++i)
            {
                delete m_cNodes[i];
            }
        }

        delete[] m_cNodes;
    }

    delete[] m_nodes;
}

e_ActionType ExtrudeArmatureNodeAction::GetActionType() 
{
    return ActionType_ExtrudeArmatureNode;
}

bool ExtrudeArmatureNodeAction::Redo()
{
    m_own = false;

    for (unsigned int i = 0; i < m_nodeCount; ++i)
    {
        m_cNodes[i]->SetParent(m_nodes[i]);
    }

    return true;
}

bool ExtrudeArmatureNodeAction::Execute()
{
    if (m_cNodes == nullptr)
    {
        m_cNodes = new Object*[m_nodeCount];

        for (unsigned int i = 0; i < m_nodeCount; ++i)
        {
            m_cNodes[i] = new Object("Node", m_nodes[i]->GetRootObject());
            m_cNodes[i]->SetParent(m_nodes[i]);
        }
    }

    const glm::vec3 endAxis = m_endPos - m_startPos;
        
    const float len = glm::length(endAxis);

    if (len != 0)
    {
        const glm::vec3 scaledAxis = m_axis * len;

        const float scale = glm::dot(scaledAxis, endAxis); 

        for (unsigned int i = 0; i < m_nodeCount; ++i)
        {
            const glm::vec3 diff = m_nodes[i]->GetTransform()->Translation() - m_startPos;

            m_cNodes[i]->GetTransform()->Translation() = (m_startPos + (m_axis * scale)) + diff;
        }
    }

    return true;
}

bool ExtrudeArmatureNodeAction::Revert()
{
    m_own = true;

    for (unsigned int i = 0; i < m_nodeCount; ++i)
    {
        m_cNodes[i]->SetParent(nullptr);
    }

    return true;
}