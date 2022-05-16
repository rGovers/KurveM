#include "Actions/MoveCurveNodeAction.h"

#include <vector>

#include "CurveModel.h"
#include "LongTasks/TriangulateCurveLongTask.h"
#include "Workspace.h"

MoveCurveNodeAction::MoveCurveNodeAction(Workspace* a_workspace, const unsigned int* a_nodeIndices, unsigned int a_nodeCount, CurveModel* a_curveModel, const glm::vec3& a_startPos, const glm::vec3& a_axis, e_MirrorMode a_mirrorMode)
{
    m_workspace = a_workspace;

    m_nodeCount = a_nodeCount;

    m_nodeIndices = new unsigned int[m_nodeCount];
    m_mirroredIndices = new unsigned int*[m_nodeCount];
    m_oldPos = new glm::vec3[m_nodeCount];

    m_curveModel = a_curveModel;

    m_startPos = a_startPos;
    m_endPos = a_startPos;

    m_axis = a_axis;

    m_mirrorMode = a_mirrorMode;

    const CurveNodeCluster* nodes = m_curveModel->GetNodes();

    for (unsigned int i = 0; i < m_nodeCount; ++i)
    {
        const unsigned int index = a_nodeIndices[i];

        m_nodeIndices[i] = index;
        m_oldPos[i] = nodes[index].Nodes[0].Node.GetPosition();
        m_mirroredIndices[i] = m_curveModel->GetMirroredIndices(index, m_mirrorMode);
    }
}
MoveCurveNodeAction::~MoveCurveNodeAction()
{
    delete[] m_nodeIndices;
    delete[] m_oldPos;

    for (unsigned int i = 0; i < m_nodeCount; ++i)
    {
        delete[] m_mirroredIndices[i];
    }
    delete[] m_mirroredIndices;
}  

glm::vec3 MoveCurveNodeAction::GetMirrorMultiplier(e_MirrorMode a_mode) const
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

e_ActionType MoveCurveNodeAction::GetActionType()
{
    return ActionType_MoveCurveNode;
}

bool MoveCurveNodeAction::Redo()
{
    return Execute();
}
bool MoveCurveNodeAction::Execute()
{
    const glm::vec3 endAxis = m_endPos - m_startPos;
        
    const float len = glm::length(endAxis);

    if (len > 0)
    {
        const glm::vec3 scaledAxis = m_axis * len;

        const float scale = glm::dot(scaledAxis, endAxis); 

        const glm::vec3 sPos = m_startPos + (m_axis * scale);

        const unsigned int modelNodeCount = m_curveModel->GetNodeCount();
        CurveNodeCluster* nodes = m_curveModel->GetNodes();

        for (unsigned int i = 0; i < m_nodeCount; ++i)
        {
            const glm::vec3 diff = m_oldPos[i] - m_startPos;
            const glm::vec3 nPos = sPos + diff;

            CurveNodeCluster& node = nodes[m_nodeIndices[i]];
            for (auto iter = node.Nodes.begin(); iter != node.Nodes.end(); ++iter)
            {
                iter->Node.SetPosition(nPos);
            }

            for (int j = 0; j < 7; ++j)
            {
                const unsigned int index = m_mirroredIndices[i][j];
                if (index != -1)
                {
                    const e_MirrorMode mirrorMode = (e_MirrorMode)(j + 1);
                    const glm::vec3 mul = GetMirrorMultiplier(mirrorMode);

                    const glm::vec3 mNPos = nPos * mul;

                    CurveNodeCluster& node = nodes[index];
                    for (auto iter = node.Nodes.begin(); iter != node.Nodes.end(); ++iter)
                    {
                        iter->Node.SetPosition(mNPos);
                    }
                }
            }
        }  
    }
    
    m_workspace->PushLongTask(new TriangulateCurveLongTask(m_curveModel));

    return true;
}
bool MoveCurveNodeAction::Revert()
{
    CurveNodeCluster* nodes = m_curveModel->GetNodes();

    for (unsigned int i = 0; i < m_nodeCount; ++i)
    {
        const glm::vec3& pos = m_oldPos[i];

        CurveNodeCluster& node = nodes[m_nodeIndices[i]];
        for (auto iter = node.Nodes.begin(); iter != node.Nodes.end(); ++iter)
        {
            iter->Node.SetPosition(pos);
        }

        for (int j = 0; j < 7; ++j)
        {
            const unsigned int index = m_mirroredIndices[i][j];
            if (index != -1)
            {
                const e_MirrorMode mirrorMode = (e_MirrorMode)(j + 1);
                const glm::vec3 mul = GetMirrorMultiplier(mirrorMode);

                const glm::vec3 mNPo = pos * mul;

                CurveNodeCluster& node = nodes[index];
                for (auto iter = node.Nodes.begin(); iter != node.Nodes.end(); ++iter)
                {
                    iter->Node.SetPosition(mNPo);
                }
            }
        }
    }

    m_workspace->PushLongTask(new TriangulateCurveLongTask(m_curveModel));
    
    return true;
}