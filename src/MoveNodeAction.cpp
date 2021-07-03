#include "Actions/MoveNodeAction.h"

#include <vector>

#include "CurveModel.h"
#include "LongTasks/TriangulateCurveLongTask.h"
#include "Workspace.h"

MoveNodeAction::MoveNodeAction(Workspace* a_workspace,unsigned int* a_nodeIndices, unsigned int a_nodeCount, CurveModel* a_curveModel, const glm::vec2& a_startCursorPos, const glm::vec3& a_xAxis, const glm::vec3& a_yAxis)
{
    m_workspace = a_workspace;

    m_nodeCount = a_nodeCount;

    m_nodeIndices = new unsigned int[m_nodeCount];
    m_startPos = new glm::vec3[m_nodeCount];

    m_curveModel = a_curveModel;

    m_startCursorPos = a_startCursorPos;

    m_xAxis = a_xAxis;
    m_yAxis = a_yAxis;

    const Node3Cluster* nodes = m_curveModel->GetNodes();
    for (unsigned int i = 0; i < m_nodeCount; ++i)
    {
        m_nodeIndices[i] = a_nodeIndices[i];
        m_startPos[i] = nodes[m_nodeIndices[i]].Nodes[0].GetPosition();
    }
}
MoveNodeAction::~MoveNodeAction()
{
    delete[] m_nodeIndices;
    delete[] m_startPos;
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
    const glm::vec2 diff = m_cursorPos - m_startCursorPos;

    Node3Cluster* nodes = m_curveModel->GetNodes();
    for (unsigned int i = 0; i < m_nodeCount; ++i)
    {
        for (auto iter = nodes[m_nodeIndices[i]].Nodes.begin(); iter != nodes[m_nodeIndices[i]].Nodes.end(); ++iter)
        {
            iter->SetPosition(m_startPos[i] + (m_yAxis * diff.y) + (m_xAxis * diff.x));
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
            iter->SetPosition(m_startPos[i]);
        }
    }

    m_workspace->PushLongTask(new TriangulateCurveLongTask(m_curveModel));
    
    return true;
}