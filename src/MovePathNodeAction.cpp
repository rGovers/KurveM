#include "Actions/MovePathNodeAction.h"

#include "LongTasks/TriangulatePathLongTask.h"
#include "PathModel.h"
#include "Workspace.h"

MovePathNodeAction::MovePathNodeAction(Workspace* a_workspace, const unsigned int* a_nodeIndices, unsigned int a_nodeCount, PathModel* a_pathModel, const glm::vec3& a_startPos, const glm::vec3& a_axis, e_MirrorMode a_mirrorMode)
{
    m_workspace = a_workspace;

    m_pathModel = a_pathModel;

    m_nodeCount = a_nodeCount;

    m_startPos = a_startPos;
    m_endPos = a_startPos;

    m_axis = a_axis;

    m_oldPos = new glm::vec3[m_nodeCount];
    m_nodeIndices = new unsigned int[m_nodeCount];
    m_mirrorIndices = new unsigned int*[m_nodeCount];

    const PathNodeCluster* nodes = m_pathModel->GetPathNodes();

    for (unsigned int i = 0; i < m_nodeCount; ++i)
    {
        const unsigned int index = a_nodeIndices[i];

        m_nodeIndices[i] = index;
        m_oldPos[i] = nodes[index].Nodes[0].Node.GetPosition();
        m_mirrorIndices[i] = m_pathModel->GetMirroredPathIndices(index, a_mirrorMode);
    }
}
MovePathNodeAction::~MovePathNodeAction()
{
    delete[] m_oldPos;
    delete[] m_nodeIndices;

    for (unsigned int i = 0; i < m_nodeCount; ++i)
    {
        delete[] m_mirrorIndices[i];
    }
    delete[] m_mirrorIndices;
}

glm::vec3 MovePathNodeAction::GetMirrorMultiplier(e_MirrorMode a_mode) const
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

e_ActionType MovePathNodeAction::GetActionType() const
{
    return ActionType_MovePathNode;
}

bool MovePathNodeAction::Redo()
{
    return Execute();
}
bool MovePathNodeAction::Execute()
{
    const glm::vec3 endAxis = m_endPos - m_startPos;

    const float len = glm::length(endAxis);

    if (len > 0)
    {
        const glm::vec3 scaledAxis = m_axis * len;

        const float scale = glm::dot(scaledAxis, endAxis);

        const glm::vec3 sPos = m_startPos + (m_axis * scale);

        PathNodeCluster* nodes = m_pathModel->GetPathNodes();
        for (unsigned int i = 0; i < m_nodeCount; ++i)
        {
            const glm::vec3 diff = m_oldPos[i] - m_startPos;
            const glm::vec3 pos = sPos + diff;

            PathNodeCluster& c = nodes[m_nodeIndices[i]];
            for (auto iter = c.Nodes.begin(); iter != c.Nodes.end(); ++iter)
            {
                iter->Node.SetPosition(pos);   
            }

            for (int j = 0; j < 7; ++j)
            {
                const unsigned int index = m_mirrorIndices[i][j];
                if (index != -1)
                {
                    const e_MirrorMode mode = (e_MirrorMode)(j + 1);
                    const glm::vec3 mul = GetMirrorMultiplier(mode);

                    const glm::vec3 nPos = pos * mul;

                    PathNodeCluster& c = nodes[index];
                    for (auto iter = c.Nodes.begin(); iter != c.Nodes.end(); ++iter)
                    {
                        iter->Node.SetPosition(nPos);
                    }
                }
            }
        }
    }

    m_workspace->PushLongTask(new TriangulatePathLongTask(m_pathModel));

    return true;
}
bool MovePathNodeAction::Revert()
{
    PathNodeCluster* nodes = m_pathModel->GetPathNodes();

    for (unsigned int i = 0; i < m_nodeCount; ++i)
    {
        const glm::vec3& pos = m_oldPos[i];

        PathNodeCluster& c = nodes[m_nodeIndices[i]];
        for (auto iter = c.Nodes.begin(); iter != c.Nodes.end(); ++iter)
        {
            iter->Node.SetPosition(pos);
        }

        for (int j = 0; j < 7; ++j)
        {
            const unsigned int index = m_mirrorIndices[i][j];
            if (index != -1)
            {
                const e_MirrorMode mode = (e_MirrorMode)(j + 1);
                const glm::vec3 mul = GetMirrorMultiplier(mode);

                const glm::vec3 nPos = pos * mul;

                PathNodeCluster& c = nodes[index];
                for (auto iter = c.Nodes.begin(); iter != c.Nodes.end(); ++iter)
                {
                    iter->Node.SetPosition(nPos);
                }
            }
        }
    }

    m_workspace->PushLongTask(new TriangulatePathLongTask(m_pathModel));

    return true;
}