#include "Actions/RotatePathNodeAction.h"

#include "LongTasks/TriangulatePathLongTask.h"
#include "PathModel.h"
#include "Workspace.h"

RotatePathNodeAction::RotatePathNodeAction(Workspace* a_workspace, const unsigned int* a_indices, unsigned int a_indexCount, PathModel* a_pathModel, const glm::vec3& a_startPos, const glm::vec3& a_axis, e_MirrorMode a_mirrorMode)
{
    m_workspace = a_workspace;

    m_pathModel = a_pathModel;

    m_nodeCount = a_indexCount;

    m_axis = a_axis;
    m_startPos = a_startPos;
    m_endPos = m_startPos;

    const PathNodeCluster* nodes = m_pathModel->GetPathNodes();

    m_nodeIndices = new unsigned int[m_nodeCount];
    m_startRotation = new float[m_nodeCount];
    m_mirrorIndices = new unsigned int*[m_nodeCount];

    for (unsigned int i = 0; i < m_nodeCount; ++i)
    {
        const unsigned int index = a_indices[i];
        m_nodeIndices[i] = index;
        m_startRotation[i] = nodes[index].Nodes[0].Rotation;
        m_mirrorIndices[i] = m_pathModel->GetMirroredPathIndices(index, a_mirrorMode);
    }
}
RotatePathNodeAction::~RotatePathNodeAction()
{
    delete[] m_nodeIndices;
    delete[] m_startRotation;

    for (unsigned int i = 0; i < m_nodeCount; ++i)
    {
        delete[] m_mirrorIndices[i];
    }
    delete[] m_mirrorIndices;
}

e_ActionType RotatePathNodeAction::GetActionType()
{
    return ActionType_RotatePathNode;
}

bool RotatePathNodeAction::Redo()
{
    return Execute();
}
bool RotatePathNodeAction::Execute()
{
    constexpr glm::vec3 vec3one = glm::vec3(1.0f);

    const glm::vec3 endAxis = m_endPos - m_startPos;

    const float len = glm::length(endAxis);

    if (len > 0)
    {
        const glm::vec3 inv = vec3one - m_axis;
        const glm::vec3 scaledAxis = inv * len;
        const float scale = glm::dot(scaledAxis, endAxis) * 10;

        PathNodeCluster* nodes = m_pathModel->GetPathNodes();

        for (unsigned int i = 0; i < m_nodeCount; ++i)
        {
            const float rot = m_startRotation[i] + scale;

            PathNodeCluster& c = nodes[m_nodeIndices[i]];
            for (auto iter = c.Nodes.begin(); iter != c.Nodes.end(); ++iter)
            {
                iter->Rotation = rot;
            }

            for (int j = 0; j < 7; ++j)
            {
                const unsigned int index = m_mirrorIndices[i][j];
                if (index != -1)
                {
                    PathNodeCluster& c = nodes[index];
                    for (auto iter = c.Nodes.begin(); iter != c.Nodes.end(); ++iter)
                    {
                        iter->Rotation = -rot;
                    }
                }
            }
        }
    }

    m_workspace->PushLongTask(new TriangulatePathLongTask(m_pathModel));

    return true;
}
bool RotatePathNodeAction::Revert()
{
    PathNodeCluster* nodes = m_pathModel->GetPathNodes();

    for (unsigned int i = 0; i < m_nodeCount; ++i)
    {
        std::vector<PathNode>& cNodes = nodes[m_nodeIndices[i]].Nodes;

        for (auto iter = cNodes.begin(); iter != cNodes.end(); ++iter)
        {
            const float rot = m_startRotation[i];

            PathNodeCluster& c = nodes[m_nodeIndices[i]];
            for (auto iter = c.Nodes.begin(); iter != c.Nodes.end(); ++iter)
            {
                iter->Rotation = rot;
            }

            for (int j = 0; j < 7; ++j)
            {
                const unsigned int index = m_mirrorIndices[i][j];
                if (index != -1)
                {
                    PathNodeCluster& c = nodes[index];
                    for (auto iter = c.Nodes.begin(); iter != c.Nodes.end(); ++iter)
                    {
                        iter->Rotation = -rot;
                    }
                }
            }
        }
    }

    m_workspace->PushLongTask(new TriangulatePathLongTask(m_pathModel));

    return true;
}