#include "Actions/ExtrudeCurveNodeAction.h"

#include "CurveModel.h"
#include "EditorControls/Editor.h"
#include "LongTasks/TriangulateCurveLongTask.h"
#include "Workspace.h"

ExtrudeCurveNodeAction::ExtrudeCurveNodeAction(Workspace* a_workspace, Editor* a_editor, const unsigned int* a_nodeIndices, unsigned int a_nodeCount, CurveModel* a_curveModel, const glm::vec3& a_startPos, const glm::vec3& a_axis)
{
    m_workspace = a_workspace;
    m_editor = a_editor;

    m_nodeCount = a_nodeCount;

    m_curveModel = a_curveModel;

    m_axis = a_axis;

    m_startPos = a_startPos;
    m_endPos = a_startPos;

    m_nodeIndices = new unsigned[m_nodeCount];

    m_startNodeIndex = -1;
    m_startFaceIndex = -1;

    for (unsigned int i = 0; i < m_nodeCount; ++i)
    {
        m_nodeIndices[i] = a_nodeIndices[i];
    }
}
ExtrudeCurveNodeAction::~ExtrudeCurveNodeAction()
{
    delete[] m_nodeIndices;
}

e_ActionType ExtrudeCurveNodeAction::GetActionType()
{
    return ActionType_ExtrudeCurveNode;
}

bool ExtrudeCurveNodeAction::Redo()
{
    return Execute();
}
bool ExtrudeCurveNodeAction::Execute()
{
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
        
        Node3Cluster* nodes = new Node3Cluster[m_nodeCount];

        for (unsigned int i = 0; i < m_nodeCount; ++i)
        {
            nodes[i] = Node3Cluster(m_curveModel->GetNode(m_nodeIndices[i]).Nodes[0].Node);

            m_editor->AddNodeToSelection(i + m_startNodeIndex);
        }

        m_curveModel->EmplaceNodes(nodes, m_nodeCount);

        delete[] nodes;
    }

    Node3Cluster* nodes = m_curveModel->GetNodes();

    for (unsigned int i = 0; i < m_nodeCount; ++i)
    {
        const unsigned int size = nodes[i + m_startNodeIndex].Nodes.size();

        const Node3Cluster startNode = nodes[m_nodeIndices[i]];

        for (unsigned int j = 0; j < size; ++j)
        {
            const glm::vec3 diff = startNode.Nodes[0].Node.GetPosition() - m_startPos;

            nodes[i + m_startNodeIndex].Nodes[j].Node.SetPosition((m_startPos + (m_axis * scale)) + diff);
            nodes[i + m_startNodeIndex].Nodes[j].Node.SetHandlePosition(glm::vec3(std::numeric_limits<float>().infinity()));
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
        m_curveModel->DestroyNodes(m_startNodeIndex, m_startNodeIndex + m_nodeCount);

        m_startNodeIndex = -1;
    }

    if (m_startFaceIndex != -1)
    {
        m_startFaceIndex = -1;
    }

    return true;
}