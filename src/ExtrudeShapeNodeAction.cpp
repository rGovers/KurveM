#include "Actions/ExtrudeShapeNodeAction.h"

#include "EditorControls/ShapeEditor.h"
#include "LongTasks/TriangulatePathLongTask.h"
#include "PathModel.h"
#include "Workspace.h"

ExtrudeShapeNodeAction::ExtrudeShapeNodeAction(Workspace* a_workspace, ShapeEditor* a_editor, const unsigned int* a_nodeIndices, unsigned int a_nodeCount, PathModel* a_pathModel, const glm::vec2& a_startPos, const glm::vec2& a_axis)
{
    m_workspace = a_workspace;
    m_editor = a_editor;

    m_model = a_pathModel;

    m_startPos = a_startPos;
    m_endPos = a_startPos;

    m_axis = a_axis;

    m_nodeCount = a_nodeCount;

    m_startNodeIndex = -1;
    m_startLineIndex = -1;

    m_nodeIndicies = new unsigned int[m_nodeCount];
    for (unsigned int i = 0; i < m_nodeCount; ++i)
    {
        m_nodeIndicies[i] = a_nodeIndices[i];
    }
}
ExtrudeShapeNodeAction::~ExtrudeShapeNodeAction()
{
    delete[] m_nodeIndicies;
}

e_ActionType ExtrudeShapeNodeAction::GetActionType()
{
    return ActionType_ExtrudeShapeNode;
}

bool ExtrudeShapeNodeAction::Redo()
{
    return Execute();
}
bool ExtrudeShapeNodeAction::Execute()
{
    if (m_nodeCount < 1)
    {
        return false;
    }

    const glm::vec2 endAxis = m_endPos - m_startPos;
    const float len = glm::length(endAxis);
    if (len > 0)
    {
        const glm::vec2 scaledAxis = m_axis * len;
        const float scale = glm::dot(scaledAxis, endAxis);
        const glm::vec2 sAxis = m_axis * scale;

        if (m_startNodeIndex == -1)
        {
            m_editor->ClearSelectedNodes();

            m_startNodeIndex = m_model->GetShapeNodeCount();

            ShapeNodeCluster* mNodes = m_model->GetShapeNodes();
            ShapeNodeCluster* nodes = new ShapeNodeCluster[m_nodeCount];
            for (unsigned int i = 0; i < m_nodeCount; ++i)
            {
                const unsigned int nodeIndex = m_nodeIndicies[i];
                const BezierCurveNode2& node = mNodes[nodeIndex].Nodes[0];
                nodes[i] = ShapeNodeCluster(node);
                mNodes[nodeIndex].Nodes.emplace_back(node);

                m_editor->AddNodeToSelection(i + m_startNodeIndex);
            }

            m_model->EmplaceShapeNodes(nodes, m_nodeCount);

            delete[] nodes;
        }

        if (m_startLineIndex == -1)
        {
            m_startLineIndex = m_model->GetShapeLineCount();

            const ShapeNodeCluster* nodes = m_model->GetShapeNodes();
            ShapeLine* lines = new ShapeLine[m_nodeCount];
            for (unsigned int i = 0; i < m_nodeCount; ++i)
            {
                const unsigned int index = m_nodeIndicies[i];

                lines[i] = ShapeLine(index, i + m_startNodeIndex, nodes[index].Nodes.size() - 1, 0);
            }

            m_model->EmplaceShapeLines(lines, m_nodeCount);

            delete[] lines;
        }

        ShapeNodeCluster* nodes = m_model->GetShapeNodes();

        for (unsigned int i = 0; i < m_nodeCount; ++i)
        {
            const unsigned int endIndex = i + m_startNodeIndex;
            const unsigned char size = (unsigned char)nodes[endIndex].Nodes.size();

            const ShapeNodeCluster &startNode = nodes[m_nodeIndicies[i]];
            const glm::vec2 diff = startNode.Nodes[0].GetPosition() - m_startPos;
            const glm::vec2 pos = m_startPos + sAxis + diff;

            for (unsigned char j = 0; j < size; ++j)
            {
                BezierCurveNode2 &node = nodes[endIndex].Nodes[j];
                node.SetPosition(pos);
                node.SetHandlePosition(pos);
            }
        }

        m_workspace->PushLongTask(new TriangulatePathLongTask(m_model));
    }

    return true;
}
bool ExtrudeShapeNodeAction::Revert()
{
    if (m_nodeCount < 1)
    {
        return false;
    }

    m_editor->ClearSelectedNodes();

    if (m_startNodeIndex != -1)
    {
        m_model->DestroyShapeNodes(m_startNodeIndex, m_startNodeIndex + m_nodeCount);

        ShapeNodeCluster* nodes = m_model->GetShapeNodes();
        for (unsigned int i = 0; i < m_nodeCount; ++i)
        {
            nodes[m_nodeIndicies[i]].Nodes.pop_back();
        }

        m_startNodeIndex = -1;
    }

    if (m_startLineIndex != -1)
    {
        m_model->DestroyShapeLines(m_startLineIndex, m_startLineIndex + m_nodeCount);

        m_startLineIndex = -1;
    }

    m_workspace->PushLongTask(new TriangulatePathLongTask(m_model));

    return true;
}