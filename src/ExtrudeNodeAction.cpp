#include "Actions/ExtrudeNodeAction.h"

#include "CurveModel.h"
#include "Editor.h"
#include "LongTasks/TriangulateCurveLongTask.h"
#include "Workspace.h"

ExtrudeNodeAction::ExtrudeNodeAction(Workspace* a_workspace, Editor* a_editor, unsigned int* a_nodeIndices, unsigned int a_nodeCount, CurveModel* a_curveModel, const glm::vec2& a_startCursorPos, const glm::vec3& a_xAxis, const glm::vec3& a_yAxis)
{
    m_workspace = a_workspace;
    m_editor = a_editor;

    m_nodeCount = a_nodeCount;

    m_curveModel = a_curveModel;

    m_xAxis = a_xAxis;
    m_yAxis = a_yAxis;

    m_startCursorPos = a_startCursorPos;

    m_nodeIndices = new unsigned[m_nodeCount];

    m_startNodeIndex = -1;
    m_startFaceIndex = -1;

    for (unsigned int i = 0; i < m_nodeCount; ++i)
    {
        m_nodeIndices[i] = a_nodeIndices[i];
    }
}
ExtrudeNodeAction::~ExtrudeNodeAction()
{

}

e_ActionType ExtrudeNodeAction::GetActionType()
{
    return ActionType_ExtrudeNode;
}

bool ExtrudeNodeAction::Redo()
{
    return Execute();
}
bool ExtrudeNodeAction::Execute()
{
    if (m_nodeCount < 1)
    {
        return false;
    }

    const glm::vec2 diff = m_cursorPos - m_startCursorPos;
    
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
    }

    Node3Cluster* nodes = m_curveModel->GetNodes();

    for (unsigned int i = 0; i < m_nodeCount; ++i)
    {
        const unsigned int size = nodes[i + m_startNodeIndex].Nodes.size();

        const Node3Cluster startNode = nodes[m_nodeIndices[i]];

        for (unsigned int j = 0; j < size; ++j)
        {
            const glm::vec3 startPos = startNode.Nodes[0].Node.GetPosition();

            nodes[i + m_startNodeIndex].Nodes[j].Node.SetPosition(startPos + (m_yAxis * diff.y) + (m_xAxis * diff.x));
            nodes[i + m_startNodeIndex].Nodes[j].Node.SetHandlePosition(glm::vec3(std::numeric_limits<float>().infinity()));
        }
    }

    m_workspace->PushLongTask(new TriangulateCurveLongTask(m_curveModel));

    return true;
}
bool ExtrudeNodeAction::Revert()
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