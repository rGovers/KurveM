#include "Actions/ExtrudeCurveNodeAction.h"

#include "CurveModel.h"
#include "EditorControls/Editor.h"
#include "LongTasks/TriangulateCurveLongTask.h"
#include "Workspace.h"

ExtrudeCurveNodeAction::ExtrudeCurveNodeAction(Workspace* a_workspace, Editor* a_editor, const unsigned int* a_nodeIndices, unsigned int a_nodeCount, CurveModel* a_curveModel, const glm::vec3& a_startPos, const glm::vec3& a_axis, e_MirrorMode a_mirrorMode)
{
    m_workspace = a_workspace;
    m_editor = a_editor;

    m_nodeCount = a_nodeCount;

    m_curveModel = a_curveModel;

    m_axis = a_axis;

    m_startPos = a_startPos;
    m_endPos = a_startPos;

    m_nodeIndices = new unsigned int[m_nodeCount];
    m_mirrorIndices = new unsigned int*[m_nodeCount];

    m_mirrorMode = a_mirrorMode;

    m_startNodeIndex = -1;
    m_startFaceIndex = -1;

    for (unsigned int i = 0; i < m_nodeCount; ++i)
    {
        const unsigned int index = a_nodeIndices[i];

        m_nodeIndices[i] = index;
        m_mirrorIndices[i] = m_curveModel->GetMirroredIndices(index, m_mirrorMode);
    }
}
ExtrudeCurveNodeAction::~ExtrudeCurveNodeAction()
{
    delete[] m_nodeIndices;

    for (unsigned int i = 0; i < m_nodeCount; ++i)
    {
        delete[] m_mirrorIndices[i];
    }
    delete[] m_mirrorIndices;
}

glm::vec3 ExtrudeCurveNodeAction::GetMirrorMultiplier(e_MirrorMode a_mode) const
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

e_ActionType ExtrudeCurveNodeAction::GetActionType() const
{
    return ActionType_ExtrudeCurveNode;
}

bool ExtrudeCurveNodeAction::Redo()
{
    return Execute();
}
bool ExtrudeCurveNodeAction::Execute()
{
    constexpr float infinity = std::numeric_limits<float>::infinity();
    constexpr glm::vec3 infinity3 = glm::vec3(infinity);

    if (m_nodeCount < 1)
    {
        return false;
    }

    const glm::vec3 endAxis = m_endPos - m_startPos;
        
    const float len = glm::length(endAxis);

    const glm::vec3 scaledAxis = m_axis * len;
    const float scale = glm::dot(scaledAxis, endAxis); 
    
    if (m_startNodeIndex == -1)
    {
        m_editor->ClearSelectedNodes();

        m_startNodeIndex = m_curveModel->GetNodeCount();
        
        CurveNodeCluster* nodes = new CurveNodeCluster[m_nodeCount];
        for (unsigned int i = 0; i < m_nodeCount; ++i)
        {
            nodes[i] = CurveNodeCluster(m_curveModel->GetNode(m_nodeIndices[i]).Nodes[0].Node);

            m_editor->AddNodeToSelection(i + m_startNodeIndex);
        }

        m_curveModel->EmplaceNodes(nodes, m_nodeCount);

        delete[] nodes;

        std::vector<CurveNodeCluster> mNodes;
        for (unsigned int i = 0; i < m_nodeCount; ++i)
        {
            for (int j = 0; j < 7; ++j)
            {
                const unsigned int index = m_mirrorIndices[i][j];
                if (index != -1)
                {
                    mNodes.emplace_back(CurveNodeCluster(m_curveModel->GetNode(index).Nodes[0].Node));
                }
            }
        }   

        const unsigned int size = (unsigned int)mNodes.size();
        if (size > 0)
        {
            m_curveModel->EmplaceNodes(mNodes.data(), size);
        }
    }

    const glm::vec3 sPos = m_startPos + (m_axis * scale);

    unsigned int indOff = 0;
    CurveNodeCluster* nodes = m_curveModel->GetNodes();
    for (unsigned int i = 0; i < m_nodeCount; ++i)
    {
        const unsigned int index = i + m_startNodeIndex;

        const CurveNodeCluster& startNode = nodes[m_nodeIndices[i]];

        const glm::vec3 diff = startNode.Nodes[0].Node.GetPosition() - m_startPos;
        const glm::vec3 pos = sPos + diff;

        CurveNodeCluster& c = nodes[index];
        for (auto iter = c.Nodes.begin(); iter != c.Nodes.end(); ++iter)
        {
            iter->Node.SetPosition(pos);
            iter->Node.SetHandlePosition(infinity3);
        }

        for (int j = 0; j < 7; ++j)
        {
            const unsigned int index = m_mirrorIndices[i][j];
            if (index != -1)
            {
                const unsigned int mInd = m_startNodeIndex + m_nodeCount + indOff++;

                const e_MirrorMode mode = (e_MirrorMode)(j + 1);
                const glm::vec3 mul = GetMirrorMultiplier(mode);

                const glm::vec3 invPos = pos * mul;

                CurveNodeCluster& c = nodes[mInd];
                for (auto iter = c.Nodes.begin(); iter != c.Nodes.end(); ++iter)
                {
                    iter->Node.SetPosition(invPos);
                    iter->Node.SetHandlePosition(infinity3);
                }
            }
        }
    }

    m_workspace->PushLongTask(new TriangulateCurveLongTask(m_curveModel));

    return true;
}
bool ExtrudeCurveNodeAction::Revert()
{
    if (m_nodeCount < 1)
    {
        return false;
    }

    m_editor->ClearSelectedNodes();

    if (m_startNodeIndex != -1)
    {
        m_curveModel->DestroyNodes(m_startNodeIndex, m_startNodeIndex + (m_curveModel->GetNodeCount() - m_startNodeIndex));

        m_startNodeIndex = -1;
    }

    if (m_startFaceIndex != -1)
    {
        m_startFaceIndex = -1;
    }

    return true;
}