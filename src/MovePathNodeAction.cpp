#include "Actions/MovePathNodeAction.h"

#include "LongTasks/TriangulatePathLongTask.h"
#include "PathModel.h"
#include "Workspace.h"

MovePathNodeAction::MovePathNodeAction(Workspace* a_workspace, const unsigned int* a_nodeIndices, unsigned int a_nodeCount, PathModel* a_pathModel, const glm::vec3& a_startPos, const glm::vec3& a_axis)
{
    m_workspace = a_workspace;

    m_pathModel = a_pathModel;

    m_nodeCount = a_nodeCount;

    m_startPos = a_startPos;
    m_endPos = a_startPos;

    m_axis = a_axis;

    m_oldPos = new glm::vec3[m_nodeCount];
    m_nodeIndices = new unsigned int[m_nodeCount];

    const PathNodeCluster* nodes = m_pathModel->GetPathNodes();

    for (unsigned int i = 0; i < m_nodeCount; ++i)
    {
        const unsigned int index = a_nodeIndices[i];

        m_nodeIndices[i] = index;

        const PathNode& node = nodes[index].Nodes[0];

        m_oldPos[i] = node.Node.GetPosition();
    }
}
MovePathNodeAction::~MovePathNodeAction()
{
    delete[] m_oldPos;
    delete[] m_nodeIndices;
}

e_ActionType MovePathNodeAction::GetActionType() 
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

        PathNodeCluster* nodes = m_pathModel->GetPathNodes();
        for (unsigned int i = 0; i < m_nodeCount; ++i)
        {
            const glm::vec3 diff = m_oldPos[i] - m_startPos;

            std::vector<PathNode>& cNodes = nodes[m_nodeIndices[i]].Nodes;
            for (auto iter = cNodes.begin(); iter != cNodes.end(); ++iter)
            {
                iter->Node.SetPosition((m_startPos + (m_axis * scale)) + diff);   
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
        std::vector<PathNode>& cNodes = nodes[m_nodeIndices[i]].Nodes;
        for (auto iter = cNodes.begin(); iter != cNodes.end(); ++iter)
        {
            iter->Node.SetPosition(m_oldPos[i]);
        }
    }

    m_workspace->PushLongTask(new TriangulatePathLongTask(m_pathModel));

    return true;
}