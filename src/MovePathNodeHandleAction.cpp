#include "Actions/MovePathNodeHandleAction.h"

#include "LongTasks/TriangulatePathLongTask.h"
#include "PathModel.h"
#include "Workspace.h"

MovePathNodeHandleAction::MovePathNodeHandleAction(Workspace* a_workspace, unsigned int a_nodeIndex, unsigned char a_clusterIndex, PathModel* a_pathModel, const glm::vec2& a_startCursorPos, const glm::vec3& a_xAxis, const glm::vec3& a_yAxis, e_MirrorMode a_mirrorMode)
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
    m_pathModel->GetMirroredPathHandle(m_nodeIndex, m_clusterIndex, a_mirrorMode, &m_mirrorNodeIndex, &m_mirrorClusterIndex);
}
MovePathNodeHandleAction::~MovePathNodeHandleAction()
{
    delete[] m_mirrorNodeIndex;
    delete[] m_mirrorClusterIndex;
}

glm::vec3 MovePathNodeHandleAction::GetMirrorMultiplier(e_MirrorMode a_mode) const
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

    const glm::vec3 pos = m_startPos + (m_yAxis * cursorDiff.y) + (m_xAxis * cursorDiff.x);

    nodes[m_nodeIndex].Nodes[m_clusterIndex].Node.SetHandlePosition(pos);

    for (int i = 0; i < 7; ++i)
    {
        const unsigned int nodeIndex = m_mirrorNodeIndex[i];
        const unsigned int clusterIndex = m_mirrorClusterIndex[i];
        if (nodeIndex != -1 && clusterIndex != -1)
        {
            const e_MirrorMode mode = (e_MirrorMode)(i + 1);
            const glm::vec3 mul = GetMirrorMultiplier(mode);

            nodes[nodeIndex].Nodes[clusterIndex].Node.SetHandlePosition(mul * pos);
        }
    }

    m_workspace->PushLongTask(new TriangulatePathLongTask(m_pathModel));

    return true;
}
bool MovePathNodeHandleAction::Revert()
{
    PathNodeCluster* nodes = m_pathModel->GetPathNodes();

    nodes[m_nodeIndex].Nodes[m_clusterIndex].Node.SetHandlePosition(m_startPos);

    for (int i = 0; i < 7; ++i)
    {
        const unsigned int nodeIndex = m_mirrorNodeIndex[i];
        const unsigned int clusterIndex = m_mirrorClusterIndex[i];
        if (nodeIndex != -1 && clusterIndex != -1)
        {
            const e_MirrorMode mode = (e_MirrorMode)(i + 1);
            const glm::vec3 mul = GetMirrorMultiplier(mode);

            nodes[nodeIndex].Nodes[clusterIndex].Node.SetHandlePosition(mul * m_startPos);
        }
    }

    m_workspace->PushLongTask(new TriangulatePathLongTask(m_pathModel));

    return true;
}