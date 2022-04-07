#include "Actions/ExtrudePathNodeAction.h"

#include "EditorControls/Editor.h"
#include "LongTasks/TriangulatePathLongTask.h"
#include "PathModel.h"
#include "Workspace.h"

ExtrudePathNodeAction::ExtrudePathNodeAction(Workspace* a_workspace, Editor* a_editor, const unsigned int* a_nodeIndices, unsigned int a_nodeCount, PathModel* a_model, const glm::vec3& a_startPos, const glm::vec3& a_axis)
{
    m_workspace = a_workspace;
    m_editor = a_editor;

    m_model = a_model;

    m_startPos = a_startPos;
    m_endPos = m_startPos;

    m_axis = a_axis;

    m_nodeCount = a_nodeCount;

    m_startNodeIndex = -1;
    m_startLineIndex = -1;

    m_nodeIndices = new unsigned int[m_nodeCount];
    for (unsigned int i = 0; i < m_nodeCount; ++i)
    {
        m_nodeIndices[i] = a_nodeIndices[i];
    }
}
ExtrudePathNodeAction::~ExtrudePathNodeAction()
{
    delete[] m_nodeIndices;
}

e_ActionType ExtrudePathNodeAction::GetActionType()
{
    return ActionType_ExtrudePathNode;
}

bool ExtrudePathNodeAction::Redo()
{
    return Execute();
}
bool ExtrudePathNodeAction::Execute()
{
    if (m_nodeCount < 1)
    {
        return false;
    }

    const glm::vec3 endAxis = m_endPos - m_startPos;

    const float len = glm::length(endAxis);
    if (len > 0)
    {
        const glm::vec3 scaledAxis = m_axis * len;
        const float scale = glm::dot(scaledAxis, endAxis);
        const glm::vec3 sAxis = m_axis * scale;

        if (m_startNodeIndex == -1)
        {
            m_editor->ClearSelectedNodes();

            m_startNodeIndex = m_model->GetPathNodeCount();

            PathNodeCluster* mNodes = m_model->GetPathNodes();
            PathNodeCluster* nodes = new PathNodeCluster[m_nodeCount];
            for (unsigned int i = 0; i < m_nodeCount; ++i)
            {
                const unsigned int nodeIndex = m_nodeIndices[i];
                const PathNode node = mNodes[nodeIndex].Nodes[0];
                nodes[i] = PathNodeCluster(node);
                mNodes[nodeIndex].Nodes.emplace_back(node);

                m_editor->AddNodeToSelection(i + m_startNodeIndex);
            }

            m_model->EmplacePathNodes(nodes, m_nodeCount);

            delete[] nodes;
        }
        if (m_startLineIndex == -1)
        {
            m_startLineIndex = m_model->GetPathLineCount();

            const PathNodeCluster* nodes = m_model->GetPathNodes();
            PathLine* lines = new PathLine[m_nodeCount];
            for (unsigned int i = 0; i < m_nodeCount; ++i)
            {
                const unsigned int index = m_nodeIndices[i];

                lines[i] = PathLine(index, i + m_startNodeIndex, nodes[index].Nodes.size() - 1, 0);
            }

            m_model->EmplacePathLines(lines, m_nodeCount);

            delete[] lines;
        }

        PathNodeCluster* nodes = m_model->GetPathNodes();

        for (unsigned int i = 0; i < m_nodeCount; ++i)
        {
            const unsigned int endIndex = i + m_startNodeIndex;
            const unsigned int size = (unsigned int)nodes[i + m_startNodeIndex].Nodes.size();

            const PathNodeCluster& startNode = nodes[m_nodeIndices[i]];
            const glm::vec3 diff = startNode.Nodes[0].Node.GetPosition() - m_startPos;
            const glm::vec3 pos = m_startPos + sAxis + diff;

            for (unsigned char j = 0; j < size; ++j)
            {
                BezierCurveNode3& node = nodes[endIndex].Nodes[j].Node;
                node.SetPosition(pos);
                node.SetHandlePosition(pos);
            }
        }

        m_workspace->PushLongTask(new TriangulatePathLongTask(m_model));
    }

    return true;
}
bool ExtrudePathNodeAction::Revert()
{
    if (m_nodeCount < 1)
    {
        return false;
    }

    m_editor->ClearSelectedNodes();

    if (m_startNodeIndex != -1)
    {
        m_model->DestroyPathNodes(m_startNodeIndex, m_startNodeIndex + m_nodeCount);

        PathNodeCluster* nodes = m_model->GetPathNodes();
        for (unsigned int i = 0; i < m_nodeCount; ++i)
        {
            nodes[m_nodeIndices[i]].Nodes.pop_back();
        }

        m_startNodeIndex = -1;
    }

    if (m_startLineIndex != -1)
    {
        m_model->DestroyPathLines(m_startLineIndex, m_startLineIndex + m_nodeCount);

        m_startLineIndex = -1;
    }

    m_workspace->PushLongTask(new TriangulatePathLongTask(m_model));

    return true;
}