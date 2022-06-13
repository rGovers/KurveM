#include "Actions/DeletePathNodeAction.h"

#include <unordered_map>

#include "EditorControls/Editor.h"
#include "LongTasks/TriangulatePathLongTask.h"
#include "PathModel.h"
#include "Workspace.h"

unsigned char DeletePathNodeAction::GetClusterIndex(const PathNode& a_node, PathNodeCluster* a_newNode)
{
    constexpr float infinity = std::numeric_limits<float>::infinity();
    const glm::vec3 handlePos = a_node.Node.GetHandlePosition();

    if (a_newNode->Nodes[0].Node.GetHandlePosition().x == infinity)
    {
        a_newNode->Nodes[0].Node.SetHandlePosition(handlePos);

        return 0;
    }

    const unsigned char size = (unsigned char)a_newNode->Nodes.size();
    for (unsigned char i = 0; i < size; ++i)
    {
        if (handlePos == a_newNode->Nodes[i].Node.GetHandlePosition())
        {
            return i;
        }
    }

    a_newNode->Nodes.emplace_back(a_node);

    return size;
}

DeletePathNodeAction::DeletePathNodeAction(Workspace* a_workspace, Editor* a_editor, const unsigned int* a_nodeIndices, unsigned int a_nodeCount, PathModel* a_model)
{
    constexpr float infinity = std::numeric_limits<float>::infinity();

    m_workspace = a_workspace;
    m_editor = a_editor;

    m_pathModel = a_model;

    m_oldNodes = m_pathModel->GetPathNodes();
    m_oldLines = m_pathModel->GetPathLines();

    m_oldNodeCount = m_pathModel->GetPathNodeCount();
    m_oldLineCount = m_pathModel->GetPathLineCount();

    std::unordered_map<unsigned int, unsigned int> lookup;

    m_nodes = new PathNodeCluster[m_oldNodeCount];

    m_nodeCount = 0;
    for (unsigned int i = 0; i < m_oldNodeCount; ++i)
    {
        PathNode node;
        for (unsigned int j = 0; j < a_nodeCount; ++j)
        {
            if (a_nodeIndices[j] == i)
            {
                goto NextNode;
            }
        }

        node = m_oldNodes[i].Nodes[0];
        node.Node.SetHandlePosition(glm::vec3(infinity));

        m_nodes[m_nodeCount] = node;

        lookup.emplace(i, m_nodeCount++);

NextNode:;
    } 

    m_lines = new PathLine[m_oldLineCount];

    m_lineCount = 0;
    for (unsigned int i = 0; i < m_oldLineCount; ++i)
    {
        const PathLine& line = m_oldLines[i];
        PathLine newLine;

        for (int j = 0; j < 2; ++j)
        {
            if (lookup.find(line.Index[j]) == lookup.end())
            {
                goto NextFace;
            }
        }

        for (int j = 0; j < 2; ++j)
        {
            const unsigned int index = line.Index[j];
            const unsigned int newIndex = lookup[index];

            newLine.Index[j] = newIndex;
            newLine.ClusterIndex[j] = GetClusterIndex(m_oldNodes[index].Nodes[line.ClusterIndex[j]], &m_nodes[newIndex]);
        }

        m_lines[m_lineCount++] = newLine;

NextFace:;
    }
}
DeletePathNodeAction::~DeletePathNodeAction()
{
    delete[] m_oldNodes;
    delete[] m_oldLines;
    delete[] m_nodes;
    delete[] m_lines;
}

e_ActionType DeletePathNodeAction::GetActionType()
{
    return ActionType_DeletePathNode;
}

bool DeletePathNodeAction::Redo()
{
    return Execute();
}
bool DeletePathNodeAction::Execute()
{
    if (m_nodeCount != m_oldNodeCount || m_lineCount != m_oldLineCount)
    {
        m_editor->ClearSelectedNodes();

        m_pathModel->SetPathModelData(m_nodes, m_nodeCount, m_lines, m_lineCount);
        
        m_workspace->PushLongTask(new TriangulatePathLongTask(m_pathModel));

        return true;
    }

    return false;
}
bool DeletePathNodeAction::Revert()
{
    if (m_nodeCount != m_oldNodeCount || m_lineCount != m_oldLineCount)
    {
        m_editor->ClearSelectedNodes();

        m_pathModel->SetPathModelData(m_oldNodes, m_oldNodeCount, m_oldLines, m_oldLineCount);
        
        m_workspace->PushLongTask(new TriangulatePathLongTask(m_pathModel));

        return true;
    }

    return false;
}