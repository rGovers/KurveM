#include "Actions/MoveCurveNodeHandleAction.h"

#include <vector>

#include "CurveModel.h"
#include "LongTasks/TriangulateCurveLongTask.h"
#include "Workspace.h"

MoveCurveNodeHandleAction::MoveCurveNodeHandleAction(Workspace* a_workspace, unsigned int a_nodeIndex, unsigned int a_clusterIndex, CurveModel* a_curveModel, const glm::vec2& a_startCursorPos, const glm::vec3& a_xAxis, const glm::vec3& a_yAxis, e_MirrorMode a_mirrorMode)
{
    m_workspace = a_workspace;

    m_nodeIndex = a_nodeIndex;
    m_clusterIndex = a_clusterIndex;

    m_curveModel = a_curveModel;

    m_startCursorPos = a_startCursorPos;

    m_xAxis = a_xAxis;
    m_yAxis = a_yAxis;

    m_mirrorMode = a_mirrorMode;

    const CurveNodeCluster* nodes = m_curveModel->GetNodes();
    m_startPos = nodes[m_clusterIndex].Nodes[m_nodeIndex].Node.GetHandlePosition();
    m_curveModel->GetMirroredHandles(m_clusterIndex, m_nodeIndex, m_mirrorMode, &m_invNodeClusterIndex, &m_invNodeIndex);
}
MoveCurveNodeHandleAction::~MoveCurveNodeHandleAction()
{
    delete[] m_invNodeIndex;
    delete[] m_invNodeClusterIndex;
}

glm::vec3 MoveCurveNodeHandleAction::GetMirrorMultiplier(e_MirrorMode a_mode) const
{
    glm::vec3 mul = glm::vec3(1.0f);

    if (a_mode & MirrorMode_X)
    {
        mul.x = -1.0f;
    }
    if (a_mode & MirrorMode_Y)
    {
        mul.y = -1.0f;
    }
    if (a_mode & MirrorMode_Z)
    {
        mul.z = -1.0f;
    }

    return mul;
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

    CurveNodeCluster* nodes = m_curveModel->GetNodes();

    const glm::vec3 pos = m_startPos + (m_yAxis * diff.y) + (m_xAxis * diff.x);

    nodes[m_clusterIndex].Nodes[m_nodeIndex].Node.SetHandlePosition(pos);

    for (int i = 0; i < 7; ++i)
    {
        const unsigned int clusterIndex = m_invNodeClusterIndex[i];
        const unsigned int nodeIndex = m_invNodeIndex[i];

        if (clusterIndex != -1 && nodeIndex != -1)
        {
            const e_MirrorMode mode = (e_MirrorMode)(i + 1);
            const glm::vec3 mul = GetMirrorMultiplier(mode);

            nodes[clusterIndex].Nodes[nodeIndex].Node.SetHandlePosition(pos * mul);
        }
    }

    m_workspace->PushLongTask(new TriangulateCurveLongTask(m_curveModel));

    return true;
}
bool MoveCurveNodeHandleAction::Revert()
{
    CurveNodeCluster* nodes = m_curveModel->GetNodes();

    nodes[m_clusterIndex].Nodes[m_nodeIndex].Node.SetHandlePosition(m_startPos);

    for (int i = 0; i < 7; ++i)
    {
        const unsigned int clusterIndex = m_invNodeClusterIndex[i];
        const unsigned int nodeIndex = m_invNodeIndex[i];

        if (clusterIndex != -1 && nodeIndex != -1)
        {
            const e_MirrorMode mode = (e_MirrorMode)(i + 1);
            const glm::vec3 mul = GetMirrorMultiplier(mode);

            nodes[clusterIndex].Nodes[nodeIndex].Node.SetHandlePosition(m_startPos * mul);
        }
    }

    m_workspace->PushLongTask(new TriangulateCurveLongTask(m_curveModel));
    
    return true;
}