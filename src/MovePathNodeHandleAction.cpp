#include "Actions/MovePathNodeHandleAction.h"

#include "LongTasks/TriangulatePathLongTask.h"
#include "PathModel.h"
#include "Workspace.h"

MovePathNodeHandleAction::MovePathNodeHandleAction(Workspace* a_workspace, unsigned int a_nodeIndex, PathModel* a_pathModel, const glm::vec2& a_startCursorPos, const glm::vec3& a_xAxis, const glm::vec3& a_yAxis)
{   
    m_workspace = a_workspace;

    m_pathModel = a_pathModel;

    m_nodeIndex = a_nodeIndex;

    m_startCursorPos = a_startCursorPos;
    m_endCursorPos = a_startCursorPos;

    m_xAxis = a_xAxis;
    m_yAxis = a_yAxis;

    const PathNode node = m_pathModel->GetNode(m_nodeIndex);

    m_startPos = node.Node.GetHandlePosition();
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

    PathNode* nodes = m_pathModel->GetNodes();

    nodes[m_nodeIndex].Node.SetHandlePosition(m_startPos + (m_yAxis * cursorDiff.y) + (m_xAxis * cursorDiff.x));

    m_workspace->PushLongTask(new TriangulatePathLongTask(m_pathModel));

    return true;
}
bool MovePathNodeHandleAction::Revert()
{
    PathNode* nodes = m_pathModel->GetNodes();

    nodes[m_nodeIndex].Node.SetHandlePosition(m_startPos);

    m_workspace->PushLongTask(new TriangulatePathLongTask(m_pathModel));

    return true;
}