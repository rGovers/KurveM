#include "Actions/MoveUVAction.h"

#include "CurveModel.h"
#include "LongTasks/TriangulateCurveLongTask.h"
#include "Workspace.h"

MoveUVAction::MoveUVAction(Workspace* a_workspace, unsigned int* a_nodeIndices, unsigned int a_nodeCount, CurveModel* a_curveModel, const glm::vec2& a_startPos, const glm::vec2& a_axis)
{
    m_workspace = a_workspace;

    m_curveModel = a_curveModel;

    m_startPos = a_startPos;
    m_endPos = a_startPos;

    m_axis = a_axis;

    m_nodeCount = a_nodeCount;

    m_nodeIndices = new unsigned int[m_nodeCount];
    m_oldPos = new glm::vec2[m_nodeCount];

    const CurveNodeCluster* nodes = m_curveModel->GetNodes();
    for (unsigned int i = 0; i < m_nodeCount; ++i)
    {
        m_nodeIndices[i] = a_nodeIndices[i];
        m_oldPos[i] = nodes[m_nodeIndices[i]].Nodes[0].Node.GetUV();
    }
}
MoveUVAction::~MoveUVAction()
{
    delete[] m_oldPos;
    delete[] m_nodeIndices;
}

e_ActionType MoveUVAction::GetActionType()
{
    return ActionType_MoveUVAction;
}

bool MoveUVAction::Redo()
{
    return Execute();
}
bool MoveUVAction::Execute()
{
    const glm::vec2 diff = (m_endPos - m_startPos) * m_axis;

    CurveNodeCluster* nodes = m_curveModel->GetNodes();
    for (unsigned int i = 0; i < m_nodeCount; ++i)
    {
        for (auto iter = nodes[m_nodeIndices[i]].Nodes.begin(); iter != nodes[m_nodeIndices[i]].Nodes.end(); ++iter)
        {
            iter->Node.SetUV(m_oldPos[i] + diff);
        }
    }

    m_workspace->PushLongTask(new TriangulateCurveLongTask(m_curveModel));

    return true;
}
bool MoveUVAction::Revert()
{
    CurveNodeCluster* nodes = m_curveModel->GetNodes();
    for (unsigned int i = 0; i < m_nodeCount; ++i)
    {
        for (auto iter = nodes[m_nodeIndices[i]].Nodes.begin(); iter != nodes[m_nodeIndices[i]].Nodes.end(); ++iter)
        {
            iter->Node.SetUV(m_oldPos[i]);
        }
    }

    m_workspace->PushLongTask(new TriangulateCurveLongTask(m_curveModel));

    return true;
}