#include "Actions/MoveShapeNodeHandleAction.h"

#include "LongTasks/TriangulatePathLongTask.h"
#include "PathModel.h"
#include "Workspace.h"

MoveShapeNodeHandleAction::MoveShapeNodeHandleAction(Workspace* a_workspace, unsigned int a_nodeIndex, unsigned char a_clusterIndex, PathModel* a_pathModel, const glm::vec2& a_cursorPos)
{
    m_workspace = a_workspace;

    m_pathModel = a_pathModel;

    m_nodeIndex = a_nodeIndex;
    m_clusterIndex = a_clusterIndex;

    m_startCursorPos = a_cursorPos;
    m_endCursorPos = a_cursorPos;

    const ShapeNodeCluster node = m_pathModel->GetShapeNode(m_nodeIndex);

    m_startPos = node.Nodes[m_clusterIndex].GetHandlePosition();
}
MoveShapeNodeHandleAction::~MoveShapeNodeHandleAction()
{

}

e_ActionType MoveShapeNodeHandleAction::GetActionType()
{
    return ActionType_MoveShapeNodeHandle;
}

bool MoveShapeNodeHandleAction::Redo()
{
    return Execute();
}
bool MoveShapeNodeHandleAction::Execute()
{
    const glm::vec2 diff = m_endCursorPos - m_startCursorPos;

    ShapeNodeCluster* nodes = m_pathModel->GetShapeNodes();
    nodes[m_nodeIndex].Nodes[m_clusterIndex].SetHandlePosition(m_startPos + diff);

    m_workspace->PushLongTask(new TriangulatePathLongTask(m_pathModel));

    return true;
}
bool MoveShapeNodeHandleAction::Revert()
{
    ShapeNodeCluster* nodes = m_pathModel->GetShapeNodes();
    nodes[m_nodeIndex].Nodes[m_clusterIndex].SetHandlePosition(m_startPos);

    m_workspace->PushLongTask(new TriangulatePathLongTask(m_pathModel));

    return true;
}