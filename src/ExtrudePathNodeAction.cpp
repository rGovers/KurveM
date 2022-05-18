#include "Actions/ExtrudePathNodeAction.h"

#include "EditorControls/Editor.h"
#include "LongTasks/TriangulatePathLongTask.h"
#include "PathModel.h"
#include "Workspace.h"

ExtrudePathNodeAction::ExtrudePathNodeAction(Workspace* a_workspace, Editor* a_editor, const unsigned int* a_nodeIndices, unsigned int a_nodeCount, PathModel* a_model, const glm::vec3& a_startPos, const glm::vec3& a_axis, e_MirrorMode a_mirrorMode)
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
    m_mirrorIndices = new unsigned int*[m_nodeCount];

    for (unsigned int i = 0; i < m_nodeCount; ++i)
    {
        const unsigned int index = a_nodeIndices[i];

        m_nodeIndices[i] = index;
        m_mirrorIndices[i] = m_model->GetMirroredPathIndices(index, a_mirrorMode);
    }
}
ExtrudePathNodeAction::~ExtrudePathNodeAction()
{
    delete[] m_nodeIndices;

    for (unsigned int i = 0; i < m_nodeCount; ++i)
    {
        delete[] m_mirrorIndices[i];
    }
    delete[] m_mirrorIndices;
}

glm::vec3 ExtrudePathNodeAction::GetMirrorMultiplier(e_MirrorMode a_mode) const
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

            PathNodeCluster* sNodes = m_model->GetPathNodes();
            PathNodeCluster* nodes = new PathNodeCluster[m_nodeCount];
            std::vector<PathNodeCluster> mNodes;
            for (unsigned int i = 0; i < m_nodeCount; ++i)
            {
                const unsigned int nodeIndex = m_nodeIndices[i];
                const PathNode node = sNodes[nodeIndex].Nodes[0];
                nodes[i] = PathNodeCluster(node);
                sNodes[nodeIndex].Nodes.emplace_back(node);

                m_editor->AddNodeToSelection(i + m_startNodeIndex);
                for (int j = 0; j < 7; ++j)
                {
                    const unsigned int index = m_mirrorIndices[i][j];
                    if (index != -1)
                    {
                        const PathNode& node = sNodes[index].Nodes[0];
                        mNodes.emplace_back(node);
                        sNodes[index].Nodes.emplace_back(node);
                    }
                }
            }

            m_model->EmplacePathNodes(nodes, m_nodeCount);
            m_model->EmplacePathNodes(mNodes.data(), (unsigned int)mNodes.size());

            delete[] nodes;
        }
        if (m_startLineIndex == -1)
        {
            m_startLineIndex = m_model->GetPathLineCount();

            const PathNodeCluster* nodes = m_model->GetPathNodes();
            PathLine* lines = new PathLine[m_nodeCount];
            std::vector<PathLine> mLines;

            unsigned int mIndex = 0;
            for (unsigned int i = 0; i < m_nodeCount; ++i)
            {
                const unsigned int index = m_nodeIndices[i];

                lines[i] = PathLine(index, i + m_startNodeIndex, (unsigned char)(nodes[index].Nodes.size() - 1), 0);

                for (int j = 0; j < 7; ++j)
                {
                    const unsigned int index = m_mirrorIndices[i][j];
                    if (index != -1)
                    {
                        mLines.emplace_back(index, m_startNodeIndex + m_nodeCount + mIndex++, (unsigned char)(nodes[index].Nodes.size() - 1), 0);
                    }
                }
            }

            m_model->EmplacePathLines(lines, m_nodeCount);
            m_model->EmplacePathLines(mLines.data(), (unsigned int)mLines.size());

            delete[] lines;
        }

        PathNodeCluster* nodes = m_model->GetPathNodes();

        unsigned int mIndex = 0;
        for (unsigned int i = 0; i < m_nodeCount; ++i)
        {
            const unsigned int endIndex = i + m_startNodeIndex;
            const unsigned int size = (unsigned int)nodes[i + m_startNodeIndex].Nodes.size();

            PathNodeCluster& startNode = nodes[m_nodeIndices[i]];

            BezierCurveNode3& startCurve = startNode.Nodes[startNode.Nodes.size() - 1].Node;

            const glm::vec3 startPos = startCurve.GetPosition();
            const glm::vec3 diff = startPos - m_startPos;
            const glm::vec3 pos = m_startPos + sAxis + diff;

            const glm::vec3 nodeDiff = pos - startPos;
            const float len = glm::length(nodeDiff);
            const float off = len * 0.25f;
            const glm::vec3 nodeDir = nodeDiff / len;

            const glm::vec3 startHandle = startPos + nodeDiff * off;
            const glm::vec3 endHandle = pos - nodeDir * off;

            startCurve.SetHandlePosition(startHandle);

            for (unsigned char j = 0; j < size; ++j)
            {
                BezierCurveNode3& node = nodes[endIndex].Nodes[j].Node;
                node.SetPosition(pos);
                node.SetHandlePosition(endHandle);
            }

            for (int j = 0; j < 7; ++j)
            {
                const unsigned int index = m_mirrorIndices[i][j];
                if (index != -1)
                {
                    const unsigned int endIndex = m_startNodeIndex + m_nodeCount + mIndex++;
                    const unsigned char size = (unsigned char)nodes[endIndex].Nodes.size();

                    const e_MirrorMode mode = (e_MirrorMode)(j + 1);
                    const glm::vec3 mul = GetMirrorMultiplier(mode);

                    const glm::vec3 mPos = mul * pos;
                    const glm::vec3 hPos = mul * endHandle;
                    const glm::vec3 sHPos = mul * startHandle;

                    PathNodeCluster& sC = nodes[index];

                    sC.Nodes[sC.Nodes.size() - 1].Node.SetHandlePosition(sHPos);

                    PathNodeCluster& c = nodes[endIndex];

                    for (unsigned char k = 0; k < size; ++k)
                    {
                        BezierCurveNode3& node = c.Nodes[k].Node;

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
bool ExtrudePathNodeAction::Revert()
{
    if (m_nodeCount < 1)
    {
        return false;
    }

    m_editor->ClearSelectedNodes();

    if (m_startNodeIndex != -1)
    {
        m_model->DestroyPathNodes(m_startNodeIndex, m_startNodeIndex + (m_model->GetPathNodeCount() - m_startNodeIndex));

        PathNodeCluster* nodes = m_model->GetPathNodes();
        for (unsigned int i = 0; i < m_nodeCount; ++i)
        {
            nodes[m_nodeIndices[i]].Nodes.pop_back();
            for (int j = 0; j < 7; ++j)
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
        m_model->DestroyPathLines(m_startLineIndex, m_startLineIndex + (m_model->GetPathLineCount() - m_startLineIndex));

        m_startLineIndex = -1;
    }

    m_workspace->PushLongTask(new TriangulatePathLongTask(m_model));

    return true;
}