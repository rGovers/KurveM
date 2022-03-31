#include "Actions/MoveShapeHandleAction.h"

#include "LongTasks/TriangulatePathLongTask.h"
#include "PathModel.h"
#include "Workspace.h"

MoveShapeNodeHandleAction::MoveShapeNodeHandleAction(Workspace* a_workspace, unsigned int a_nodeIndex, PathModel* a_pathModel, const glm::vec2& a_cursorPos)
{
    m_workspace = a_workspace;

    m_pathModel = a_pathModel;

    m_nodeIndex = a_nodeIndex;

    m_startCursorPos = a_cursorPos;
    m_endCursorPos = a_cursorPos;

    const BezierCurveNode2* nodes = m_pathModel->GetShapeNodes();

    m_startPos = nodes[m_nodeIndex].GetHandlePosition();
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

    BezierCurveNode2* nodes = m_pathModel->GetShapeNodes();
    nodes[m_nodeIndex].SetHandlePosition(m_startPos + diff);

    m_workspace->PushLongTask(new TriangulatePathLongTask(m_pathModel));

    return true;
}
bool MoveShapeNodeHandleAction::Revert()
{
    BezierCurveNode2* nodes = m_pathModel->GetShapeNodes();
    nodes[m_nodeIndex].SetHandlePosition(m_startPos);

    m_workspace->PushLongTask(new TriangulatePathLongTask(m_pathModel));

    return true;
}