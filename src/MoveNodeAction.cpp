#include "Actions/MoveNodeAction.h"

#include <vector>

#include "CurveModel.h"
#include "LongTasks/TriangulateCurveLongTask.h"
#include "Workspace.h"

MoveNodeAction::MoveNodeAction(Workspace* a_workspace, unsigned int* a_nodeIndices, unsigned int a_nodeCount, CurveModel* a_curveModel, const glm::vec3& a_startPos, const glm::vec3& a_axis)
{
    m_workspace = a_workspace;

    m_nodeCount = a_nodeCount;

    m_nodeIndices = new unsigned int[m_nodeCount];
    m_oldPos = new glm::vec3[m_nodeCount];

    m_curveModel = a_curveModel;

    m_startPos = a_startPos;
    m_endPos = a_startPos;

    m_axis = a_axis;

    const Node3Cluster* nodes = m_curveModel->GetNodes();
    for (unsigned int i = 0; i < m_nodeCount; ++i)
    {
        m_nodeIndices[i] = a_nodeIndices[i];
        m_oldPos[i] = nodes[m_nodeIndices[i]].Nodes[0].Node.GetPosition();
    }
}
MoveNodeAction::~MoveNodeAction()
{
    delete[] m_nodeIndices;
    delete[] m_oldPos;
}  

e_ActionType MoveNodeAction::GetActionType()
{
    return ActionType_MoveNode;
}

bool MoveNodeAction::Redo()
{
    return Execute();
}
bool MoveNodeAction::Execute()
{
    const glm::vec3 endAxis = m_endPos - m_startPos;
        
    const float len = glm::length(endAxis);

    if (len != 0)
    {
        const glm::vec3 scaledAxis = m_axis * len;

        const float scale = glm::dot(scaledAxis, endAxis); 

        Node3Cluster* nodes = m_curveModel->GetNodes();
        for (unsigned int i = 0; i < m_nodeCount; ++i)
        {
            const glm::vec3 diff = m_oldPos[i] - m_startPos;

            for (auto iter = nodes[m_nodeIndices[i]].Nodes.begin(); iter != nodes[m_nodeIndices[i]].Nodes.end(); ++iter)
            {
                iter->Node.SetPosition((m_startPos + (m_axis * scale)) + diff);
            }
        }
    }
    
    m_workspace->PushLongTask(new TriangulateCurveLongTask(m_curveModel));

    return true;
}
bool MoveNodeAction::Revert()
{
    Node3Cluster* nodes = m_curveModel->GetNodes();

    for (unsigned int i = 0; i < m_nodeCount; ++i)
    {
        for (auto iter = nodes[m_nodeIndices[i]].Nodes.begin(); iter != nodes[m_nodeIndices[i]].Nodes.end(); ++iter)
        {
            iter->Node.SetPosition(m_oldPos[i]);
        }
    }

    m_workspace->PushLongTask(new TriangulateCurveLongTask(m_curveModel));
    
    return true;
}