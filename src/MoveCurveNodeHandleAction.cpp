#include "Actions/MoveCurveNodeHandleAction.h"

#include <vector>

#include "CurveModel.h"
#include "LongTasks/TriangulateCurveLongTask.h"
#include "Workspace.h"

MoveCurveNodeHandleAction::MoveCurveNodeHandleAction(Workspace* a_workspace, unsigned int a_nodeIndex, unsigned int a_clusterIndex, CurveModel* a_curveModel, const glm::vec2& a_startCursorPos, const glm::vec3& a_xAxis, const glm::vec3& a_yAxis)
{
    m_workspace = a_workspace;

    m_nodeIndex = a_nodeIndex;
    m_clusterIndex = a_clusterIndex;

    m_curveModel = a_curveModel;

    m_startCursorPos = a_startCursorPos;

    m_xAxis = a_xAxis;
    m_yAxis = a_yAxis;

    const Node3Cluster* nodes = m_curveModel->GetNodes();
    m_startPos = nodes[m_clusterIndex].Nodes[m_nodeIndex].Node.GetHandlePosition();
}
MoveCurveNodeHandleAction::~MoveCurveNodeHandleAction()
{

}

e_ActionType MoveCurveNodeHandleAction::GetActionType()
{
    return ActionType_MoveCurveNodeHandle;
}

bool MoveCurveNodeHandleAction::Redo()
{
    return Execute();
}
bool MoveCurveNodeHandleAction::Execute()
{
    const glm::vec2 diff = m_cursorPos - m_startCursorPos;

    Node3Cluster* nodes = m_curveModel->GetNodes();

    nodes[m_clusterIndex].Nodes[m_nodeIndex].Node.SetHandlePosition(m_startPos + (m_yAxis * diff.y) + (m_xAxis * diff.x));

    m_workspace->PushLongTask(new TriangulateCurveLongTask(m_curveModel));

    return true;
}
bool MoveCurveNodeHandleAction::Revert()
{
    Node3Cluster* nodes = m_curveModel->GetNodes();

    nodes[m_clusterIndex].Nodes[m_nodeIndex].Node.SetHandlePosition(m_startPos);

    m_workspace->PushLongTask(new TriangulateCurveLongTask(m_curveModel));
    
    return true;
}