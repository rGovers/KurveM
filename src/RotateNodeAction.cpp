#include "Actions/RotateNodeAction.h"

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "CurveModel.h"
#include "LongTasks/TriangulateCurveLongTask.h"
#include "Workspace.h"

RotateNodeAction::RotateNodeAction(Workspace* a_workspace, const unsigned int* a_nodeIndices, unsigned int a_nodeCount, CurveModel* a_curveModel, const glm::vec3& a_startPos, const glm::vec3& a_axis)
{
    m_workspace = a_workspace;

    m_axis = a_axis;

    m_startPos = a_startPos;
    m_endPos = a_startPos;

    m_nodeCount = a_nodeCount;

    m_curveModel = a_curveModel;

    m_oldPos = new glm::vec3[m_nodeCount];
    m_nodeIndices = new unsigned int[m_nodeCount];

    const Node3Cluster* nodes = m_curveModel->GetNodes();

    m_centre = glm::vec3(0);

    for (unsigned int i = 0; i < m_nodeCount; ++i) 
    {
        const unsigned int index = a_nodeIndices[i];

        m_nodeIndices[i] = index;

        const glm::vec3 pos = nodes[index].Nodes[0].Node.GetPosition();

        m_oldPos[i] = pos;
        m_centre += pos; 
    }

    m_centre /= m_nodeCount;
}
RotateNodeAction::~RotateNodeAction() 
{
    delete[] m_nodeIndices;

    delete[] m_oldPos; 
}

e_ActionType RotateNodeAction::GetActionType()
{
    return ActionType_RotateNode;
}

bool RotateNodeAction::Redo()
{
    return Execute();
}
bool RotateNodeAction::Execute()
{
    const glm::vec3 endAxis = m_endPos - m_startPos;

    const float len = glm::length(endAxis);

    if (len > 0)
    {
        const glm::vec3 inv = glm::vec3(1) - m_axis;

        const glm::vec3 scaledAxis = inv * len;
        
        const float scale = glm::dot(scaledAxis, endAxis) * 10;

        const glm::quat q = glm::angleAxis(scale, m_axis);

        Node3Cluster* nodes = m_curveModel->GetNodes();

        for (unsigned int i = 0; i < m_nodeCount; ++i)
        {
            const glm::vec3 diff = m_oldPos[i] - m_centre;
            const glm::vec3 pos = m_centre + (q * diff);

            const unsigned int index = m_nodeIndices[i];
            const unsigned int nodeSize = nodes[index].Nodes.size();

            for (unsigned int j = 0; j < nodeSize; ++j)
            {
                nodes[index].Nodes[j].Node.SetPosition(pos);
            }
        }
    }

    m_workspace->PushLongTask(new TriangulateCurveLongTask(m_curveModel));

    return true;
}
bool RotateNodeAction::Revert()
{
    Node3Cluster* nodes = m_curveModel->GetNodes();

    for (unsigned int i = 0; i < m_nodeCount; ++i)
    {
        const unsigned int nodeIndex = m_nodeIndices[i];
        const unsigned int nodeSize = nodes[nodeIndex].Nodes.size();

        const glm::vec3 pos = m_oldPos[i];
        for (unsigned int j = 0; j < nodeSize; ++j)
        {
            nodes[nodeIndex].Nodes[j].Node.SetPosition(pos);
        }
    }

    m_workspace->PushLongTask(new TriangulateCurveLongTask(m_curveModel));

    return true;
}