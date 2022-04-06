#include "Actions/MovePathNodeHandleAction.h"

#include "LongTasks/TriangulatePathLongTask.h"
#include "PathModel.h"
#include "Workspace.h"

MovePathNodeHandleAction::MovePathNodeHandleAction(Workspace* a_workspace, unsigned int a_nodeIndex, unsigned char a_clusterIndex, PathModel* a_pathModel, const glm::vec2& a_startCursorPos, const glm::vec3& a_xAxis, const glm::vec3& a_yAxis)
{   
    m_workspace = a_workspace;

    m_pathModel = a_pathModel;

    m_nodeIndex = a_nodeIndex;
    m_clusterIndex = a_clusterIndex;

    m_startCursorPos = a_startCursorPos;
    m_endCursorPos = a_startCursorPos;

    m_xAxis = a_xAxis;
    m_yAxis = a_yAxis;

    const PathNodeCluster node = m_pathModel->GetPathNode(m_nodeIndex);

    m_startPos = node.Nodes[m_clusterIndex].Node.GetHandlePosition();
}
MovePathNodeHandleAction::~MovePathNodeHandleAction()
{

}

e_ActionType MovePathNodeHandleAction::GetActionType()
{
    return ActionType_MovePathNodeHandle;
}

bool MovePathNodeHandleAction::Redo()
{
    return Execute();
}
bool MovePathNodeHandleAction::Execute()
{
    const glm::vec2 cursorDiff = m_endCursorPos - m_startCursorPos;

    PathNodeCluster* nodes = m_pathModel->GetPathNodes();

    nodes[m_nodeIndex].Nodes[m_clusterIndex].Node.SetHandlePosition(m_startPos + (m_yAxis * cursorDiff.y) + (m_xAxis * cursorDiff.x));

    m_workspace->PushLongTask(new TriangulatePathLongTask(m_pathModel));

    return true;
}
bool MovePathNodeHandleAction::Revert()
{
    PathNodeCluster* nodes = m_pathModel->GetPathNodes();

    nodes[m_nodeIndex].Nodes[m_clusterIndex].Node.SetHandlePosition(m_startPos);

    m_workspace->PushLongTask(new TriangulatePathLongTask(m_pathModel));

    return true;
}