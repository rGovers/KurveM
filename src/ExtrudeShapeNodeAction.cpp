#include "Actions/ExtrudeShapeNodeAction.h"

#include "EditorControls/ShapeEditor.h"
#include "LongTasks/TriangulatePathLongTask.h"
#include "PathModel.h"
#include "Workspace.h"

ExtrudeShapeNodeAction::ExtrudeShapeNodeAction(Workspace* a_workspace, ShapeEditor* a_editor, const unsigned int* a_nodeIndices, unsigned int a_nodeCount, PathModel* a_pathModel, const glm::vec2& a_startPos, const glm::vec2& a_axis, e_MirrorMode a_mirrorMode)
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
    m_mirrorIndices = new unsigned int*[m_nodeCount];

    for (unsigned int i = 0; i < m_nodeCount; ++i)
    {
        const unsigned int index = a_nodeIndices[i];

        m_nodeIndicies[i] = index;
        m_mirrorIndices[i] = m_model->GetMirroredShapeIndices(index, a_mirrorMode);
    }
}
ExtrudeShapeNodeAction::~ExtrudeShapeNodeAction()
{
    delete[] m_nodeIndicies;

    for (unsigned int i = 0; i < m_nodeCount; ++i)
    {
        delete[] m_mirrorIndices[i];
    }
    delete[] m_mirrorIndices;
}

glm::vec2 ExtrudeShapeNodeAction::GetMirrorMultiplier(e_MirrorMode a_mode) const
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

            ShapeNodeCluster* sNodes = m_model->GetShapeNodes();
            ShapeNodeCluster* nodes = new ShapeNodeCluster[m_nodeCount];
            std::vector<ShapeNodeCluster> mNodes;
            for (unsigned int i = 0; i < m_nodeCount; ++i)
            {
                const unsigned int nodeIndex = m_nodeIndicies[i];
                const BezierCurveNode2& node = sNodes[nodeIndex].Nodes[0];
                nodes[i] = ShapeNodeCluster(node);
                sNodes[nodeIndex].Nodes.emplace_back(node);

                m_editor->AddNodeToSelection(i + m_startNodeIndex);
                for (int j = 0; j < 3; ++j)
                {
                    const unsigned int index = m_mirrorIndices[i][j];
                    if (index != -1)
                    {
                        const BezierCurveNode2& node = sNodes[index].Nodes[0];
                        mNodes.emplace_back(node);
                        sNodes[index].Nodes.emplace_back(node);
                    }
                }
            }

            m_model->EmplaceShapeNodes(nodes, m_nodeCount);
            m_model->EmplaceShapeNodes(mNodes.data(), (unsigned int)mNodes.size());

            delete[] nodes;
        }

        if (m_startLineIndex == -1)
        {
            m_startLineIndex = m_model->GetShapeLineCount();

            const ShapeNodeCluster* nodes = m_model->GetShapeNodes();
            ShapeLine* lines = new ShapeLine[m_nodeCount];
            std::vector<ShapeLine> mLines;

            unsigned int mIndex = 0;
            for (unsigned int i = 0; i < m_nodeCount; ++i)
            {
                const unsigned int index = m_nodeIndicies[i];

                lines[i] = ShapeLine(index, i + m_startNodeIndex, (unsigned char)(nodes[index].Nodes.size() - 1), 0);

                for (int j = 0; j < 3; ++j)
                {
                    const unsigned int index = m_mirrorIndices[i][j];
                    if (index != -1)
                    {
                        mLines.emplace_back(index, m_startNodeIndex + m_nodeCount + mIndex++, (unsigned char)(nodes[index].Nodes.size() - 1), 0);
                    }
                }
            }

            m_model->EmplaceShapeLines(lines, m_nodeCount);
            m_model->EmplaceShapeLines(mLines.data(), (unsigned int)mLines.size());

            delete[] lines;
        }

        ShapeNodeCluster* nodes = m_model->GetShapeNodes();

        unsigned int mIndex = 0;
        for (unsigned int i = 0; i < m_nodeCount; ++i)
        {
            const unsigned int endIndex = i + m_startNodeIndex;
            const unsigned char size = (unsigned char)nodes[endIndex].Nodes.size();

            ShapeNodeCluster& startNode = nodes[m_nodeIndicies[i]];

            BezierCurveNode2& startCurve = startNode.Nodes[startNode.Nodes.size() - 1];

            const glm::vec2 startPos = startCurve.GetPosition();
            const glm::vec2 diff = startPos - m_startPos;
            const glm::vec2 pos = m_startPos + sAxis + diff;

            const glm::vec2 nodeDiff = pos - startPos;
            const float len = glm::length(nodeDiff);
            const float off = len * 0.25f;
            const glm::vec2 nodeDir = nodeDiff / len;

            const glm::vec2 startHandle = startPos + nodeDiff * off;
            const glm::vec2 endHandle = pos - nodeDir * off;

            startCurve.SetHandlePosition(startHandle);

            for (unsigned char j = 0; j < size; ++j)
            {
                BezierCurveNode2& node = nodes[endIndex].Nodes[j];

                node.SetPosition(pos);
                node.SetHandlePosition(endHandle);
            }

            for (int j = 0; j < 3; ++j)
            {
                const unsigned int index = m_mirrorIndices[i][j];
                if (index != -1)
                {
                    const unsigned int endIndex = m_startNodeIndex + m_nodeCount + mIndex++;
                    const unsigned char size = (unsigned char)nodes[endIndex].Nodes.size();

                    const e_MirrorMode mode = (e_MirrorMode)(j + 1);
                    const glm::vec2 mul = GetMirrorMultiplier(mode);

                    const glm::vec2 mPos = mul * pos;
                    const glm::vec2 hPos = mul * endHandle;
                    const glm::vec2 sHPos = mul * startHandle;

                    ShapeNodeCluster& sC = nodes[index];

                    sC.Nodes[sC.Nodes.size() - 1].SetHandlePosition(sHPos);

                    ShapeNodeCluster& c = nodes[endIndex];

                    for (unsigned char k = 0; k < size; ++k)
                    {
                        BezierCurveNode2& node = c.Nodes[k];

                        node.SetPosition(mPos);
                        node.SetHandlePosition(hPos); 
                    } 
                }
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
        m_model->DestroyShapeNodes(m_startNodeIndex, m_startNodeIndex + (m_model->GetShapeNodeCount() - m_startNodeIndex));

        ShapeNodeCluster* nodes = m_model->GetShapeNodes();
        for (unsigned int i = 0; i < m_nodeCount; ++i)
        {
            nodes[m_nodeIndicies[i]].Nodes.pop_back();
            for (int j = 0; j < 3; ++j)
            {
                const unsigned int index = m_mirrorIndices[i][j];
                if (index != -1)
                {
                    nodes[index].Nodes.pop_back();
                }
            }
        }

        m_startNodeIndex = -1;
    }

    if (m_startLineIndex != -1)
    {
        m_model->DestroyShapeLines(m_startLineIndex, m_startLineIndex + (m_model->GetShapeLineCount() - m_startLineIndex));

        m_startLineIndex = -1;
    }

    m_workspace->PushLongTask(new TriangulatePathLongTask(m_model));

    return true;
}