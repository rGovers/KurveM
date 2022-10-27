#include "Actions/MoveShapeNodeHandleAction.h"

#include "LongTasks/TriangulatePathLongTask.h"
#include "PathModel.h"
#include "Workspace.h"

MoveShapeNodeHandleAction::MoveShapeNodeHandleAction(Workspace* a_workspace, unsigned int a_nodeIndex, unsigned char a_clusterIndex, PathModel* a_pathModel, const glm::vec2& a_cursorPos, e_MirrorMode a_mirrorMode)
{
    m_workspace = a_workspace;

    m_pathModel = a_pathModel;

    m_nodeIndex = a_nodeIndex;
    m_clusterIndex = a_clusterIndex;

    m_startCursorPos = a_cursorPos;
    m_endCursorPos = a_cursorPos;

    const ShapeNodeCluster node = m_pathModel->GetShapeNode(m_nodeIndex);

    m_startPos = node.Nodes[m_clusterIndex].GetHandlePosition();

    m_pathModel->GetMirroredShapeHandle(m_nodeIndex, m_clusterIndex, a_mirrorMode, &m_mirroredIndex, &m_mirroredClusterIndex);
}
MoveShapeNodeHandleAction::~MoveShapeNodeHandleAction()
{
    delete[] m_mirroredIndex;
    delete[] m_mirroredClusterIndex;
}

glm::vec2 MoveShapeNodeHandleAction::GetMirrorMultiplier(e_MirrorMode a_mode) const
{
    glm::vec2 mul = glm::vec3(1.0f);
    if (a_mode & MirrorMode_X)
    {
        mul.x = -1.0f;
    }
    if (a_mode & MirrorMode_Y)
    {
        mul.y = -1.0f;
    }

    return mul;
}

e_ActionType MoveShapeNodeHandleAction::GetActionType() const
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

    const glm::vec2 pos = m_startPos + diff;

    nodes[m_nodeIndex].Nodes[m_clusterIndex].SetHandlePosition(pos);

    for (int i = 0; i < 3; ++i)
    {
        const unsigned int nodeIndex = m_mirroredIndex[i];
        const unsigned char clusterIndex = m_mirroredClusterIndex[i];
        if (nodeIndex != -1 && m_clusterIndex != -1)
        {
            const e_MirrorMode mode = (e_MirrorMode)(i + 1);
            const glm::vec2 mul = GetMirrorMultiplier(mode);

            nodes[nodeIndex].Nodes[clusterIndex].SetHandlePosition(mul * pos);
        }
    }

    m_workspace->PushLongTask(new TriangulatePathLongTask(m_pathModel));

    return true;
}
bool MoveShapeNodeHandleAction::Revert()
{
    ShapeNodeCluster* nodes = m_pathModel->GetShapeNodes();

    nodes[m_nodeIndex].Nodes[m_clusterIndex].SetHandlePosition(m_startPos);

    for (int i = 0; i < 3; ++i)
    {
        const unsigned int nodeIndex = m_mirroredIndex[i];
        const unsigned char clusterIndex = m_mirroredClusterIndex[i];
        if (nodeIndex != -1 && m_clusterIndex != -1)
        {
            const e_MirrorMode mode = (e_MirrorMode)(i + 1);
            const glm::vec2 mul = GetMirrorMultiplier(mode);

            nodes[nodeIndex].Nodes[clusterIndex].SetHandlePosition(mul * m_startPos);
        }
    }

    m_workspace->PushLongTask(new TriangulatePathLongTask(m_pathModel));

    return true;
}