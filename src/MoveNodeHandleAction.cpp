#include "Actions/MoveNodeHandleAction.h"

#include <vector>

#include "CurveModel.h"

MoveNodeHandleAction::MoveNodeHandleAction(unsigned int a_nodeIndex, unsigned int a_clusterIndex, CurveModel* a_curveModel, const glm::vec2& a_startCursorPos, const glm::vec3& a_xAxis, const glm::vec3& a_yAxis)
{
    m_nodeIndex = a_nodeIndex;
    m_clusterIndex = a_clusterIndex;

    m_curveModel = a_curveModel;

    m_startCursorPos = a_startCursorPos;

    m_xAxis = a_xAxis;
    m_yAxis = a_yAxis;

    const Node3Cluster* nodes = m_curveModel->GetNodes();
    m_startPos = nodes[m_clusterIndex].Nodes[m_nodeIndex].GetHandlePosition();
}
MoveNodeHandleAction::~MoveNodeHandleAction()
{

}

e_ActionType MoveNodeHandleAction::GetActionType()
{
    return ActionType_MoveNodeHandle;
}

bool MoveNodeHandleAction::Redo()
{
    return Execute();
}
bool MoveNodeHandleAction::Execute()
{
    const glm::vec2 diff = m_cursorPos - m_startCursorPos;

    Node3Cluster* nodes = m_curveModel->GetNodes();

    nodes[m_clusterIndex].Nodes[m_nodeIndex].SetHandlePosition(m_startPos + (m_yAxis * diff.y) + (m_xAxis * diff.x));

    m_curveModel->Triangulate();

    return true;
}
bool MoveNodeHandleAction::Revert()
{
    Node3Cluster* nodes = m_curveModel->GetNodes();

    nodes[m_clusterIndex].Nodes[m_nodeIndex].SetHandlePosition(m_startPos);

    m_curveModel->Triangulate();
    
    return true;
}