#include "Actions/InsertPathLineAction.h"

#include "LongTasks/TriangulatePathLongTask.h"
#include "PathModel.h"
#include "Workspace.h"

InsertPathLineAction::InsertPathLineAction(Workspace* a_workspace, const unsigned int* a_nodeIndices, unsigned int a_nodeCount, PathModel* a_model)
{
    m_workspace = a_workspace;

    m_pathModel = a_model;

    m_lineIndex = -1;

    m_nodeCount = a_nodeCount;
    m_nodeIndices = new unsigned int[m_nodeCount];
    for (unsigned int i = 0; i < m_nodeCount; ++i)
    {
        m_nodeIndices[i] = a_nodeIndices[i];
    }
}
InsertPathLineAction::~InsertPathLineAction()
{
    delete[] m_nodeIndices;
}

e_ActionType InsertPathLineAction::GetActionType()
{
    return ActionType_InsertPathLine;
}

unsigned char InsertPathLineAction::PushNode(PathNodeCluster* a_cluster)
{
    constexpr float infinity = std::numeric_limits<float>::infinity();

    const unsigned char size = (unsigned char)a_cluster->Nodes.size();
    for (unsigned char i = 0; i < size; ++i)
    {
        PathNode& node = a_cluster->Nodes[i];
        if (node.Node.GetHandlePosition().x == infinity)
        {
            node.Node.SetHandlePosition(node.Node.GetPosition());

            return i;
        }
    }

    PathNode node = a_cluster->Nodes[0];
    node.Node.SetHandlePosition(node.Node.GetPosition());
    a_cluster->Nodes.emplace_back(node);

    return size;
}

bool InsertPathLineAction::Redo()
{
    return Execute();
}
bool InsertPathLineAction::Execute()
{
    if (m_nodeCount == 2)
    {
        const unsigned int indexA = m_nodeIndices[0];
        const unsigned int indexB = m_nodeIndices[1];

        PathNodeCluster* nodes = m_pathModel->GetPathNodes();
        const PathLine line = PathLine(indexA, indexB, PushNode(&nodes[indexA]), PushNode(&nodes[indexB]));

        m_lineIndex = m_pathModel->GetPathLineCount();

        m_pathModel->EmplacePathLine(line);

        m_workspace->PushLongTask(new TriangulatePathLongTask(m_pathModel));

        return true;
    }

    return false;
}
bool InsertPathLineAction::Revert()
{
    constexpr float infinity = std::numeric_limits<float>::infinity();

    if (m_nodeCount == 2)
    {
        const unsigned int lineCount = m_pathModel->GetPathLineCount();

        const PathLine* lines = m_pathModel->GetPathLines();
        const PathLine& line = lines[m_lineIndex];
        PathNodeCluster* nodes = m_pathModel->GetPathNodes();

        nodes[line.Index[0]].Nodes[line.ClusterIndex[0]].Node.SetHandlePosition(glm::vec3(infinity));
        nodes[line.Index[1]].Nodes[line.ClusterIndex[1]].Node.SetHandlePosition(glm::vec3(infinity));

        m_pathModel->DestroyPathLine(m_lineIndex);

        m_lineIndex = -1;

        m_workspace->PushLongTask(new TriangulatePathLongTask(m_pathModel));
    }

    return true;
}