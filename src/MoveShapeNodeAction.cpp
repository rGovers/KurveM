#include "Actions/MoveShapeNodeAction.h"

#include "LongTasks/TriangulatePathLongTask.h"
#include "PathModel.h"
#include "Workspace.h"

MoveShapeNodeAction::MoveShapeNodeAction(Workspace* a_workspace, const unsigned int* a_indices, unsigned int a_indexCount, PathModel* a_pathModel, const glm::vec2& a_pos, const glm::vec2& a_axis)
{
    m_workspace = a_workspace;

    m_model = a_pathModel;

    m_startPos = a_pos;
    m_endPos = a_pos;

    m_axis = a_axis;

    const BezierCurveNode2* nodes = m_model->GetShapeNodes();

    m_indexCount = a_indexCount;
    m_indices = new unsigned int[m_indexCount];
    m_oldPos = new glm::vec2[m_indexCount];
    for (unsigned int i = 0; i < m_indexCount; ++i)
    {
        const unsigned int index = a_indices[i];
        m_indices[i] = index;
        m_oldPos[i] = nodes[index].GetPosition();
    }
}
MoveShapeNodeAction::~MoveShapeNodeAction()
{
    delete[] m_indices;
    delete[] m_oldPos;
}

e_ActionType MoveShapeNodeAction::GetActionType()
{
    return ActionType_MoveShapeNode;
}

bool MoveShapeNodeAction::Redo()
{
    return Execute();
}
bool MoveShapeNodeAction::Execute()
{
    BezierCurveNode2* nodes = m_model->GetShapeNodes();

    const glm::vec2 delta = (m_endPos - m_startPos) * m_axis;

    for (unsigned int i = 0; i < m_indexCount; ++i)
    {
        nodes[m_indices[i]].SetPosition(m_oldPos[i] + delta);
    }

    m_workspace->PushLongTask(new TriangulatePathLongTask(m_model));

    return true;
}
bool MoveShapeNodeAction::Revert()
{
    BezierCurveNode2* nodes = m_model->GetShapeNodes();

    for (unsigned int i = 0; i < m_indexCount; ++i)
    {
        nodes[m_indices[i]].SetPosition(m_oldPos[i]);
    }

    m_workspace->PushLongTask(new TriangulatePathLongTask(m_model));

    return true;
}