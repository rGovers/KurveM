#include "Actions/ScaleCurveNodeAction.h"

#include "CurveModel.h"
#include "LongTasks/TriangulateCurveLongTask.h"
#include "Workspace.h"

ScaleCurveNodeAction::ScaleCurveNodeAction(Workspace* a_workspace, const unsigned int* a_nodeIndices, unsigned int a_nodeCount, CurveModel* a_curveModel, const glm::vec3& a_startPos, const glm::vec3& a_axis, e_MirrorMode a_mirrorMode)
{
    m_workspace = a_workspace;

    m_curveModel = a_curveModel;

    m_nodeCount = a_nodeCount;

    m_startPos = a_startPos;
    m_endPos = a_startPos;

    m_axis = a_axis;

    m_centre = glm::vec3(0.0f); 

    m_nodeIndices = new unsigned int[m_nodeCount];
    m_oldPos = new glm::vec3[m_nodeCount];
    m_invNodeIndices = new unsigned int*[m_nodeCount];

    m_mirrorMode = a_mirrorMode;

    const CurveNodeCluster* nodes = m_curveModel->GetNodes(); 

    for (unsigned int i = 0; i < m_nodeCount; ++i)
    {
        const unsigned int index = a_nodeIndices[i];

        m_nodeIndices[i] = index;

        const glm::vec3 pos = nodes[index].Nodes[0].Node.GetPosition();

        m_oldPos[i] = pos;
        m_centre += pos;
        m_invNodeIndices[i] = m_curveModel->GetMirroredIndices(index, m_mirrorMode);
    }

    m_centre /= m_nodeCount;
}
ScaleCurveNodeAction::~ScaleCurveNodeAction()
{
    delete[] m_oldPos;
    delete[] m_nodeIndices;

    for (unsigned int i = 0; i < m_nodeCount; ++i)
    {
        delete[] m_invNodeIndices[i];
    }
    delete[] m_invNodeIndices;
}

glm::vec3 ScaleCurveNodeAction::GetMirrorMultiplier(e_MirrorMode a_mode) const
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

e_ActionType ScaleCurveNodeAction::GetActionType()
{
    return ActionType_ScaleCurveNode;
}

bool ScaleCurveNodeAction::Redo()
{
    return Execute();
}
bool ScaleCurveNodeAction::Execute()
{
    const glm::vec3 endAxis = m_endPos - m_startPos;
        
    const float len = glm::length(endAxis);

    if (len > 0)
    {
        const glm::vec3 scaledAxis = m_axis * len;

        const float scale = glm::dot(scaledAxis, endAxis); 

        CurveNodeCluster* nodes = m_curveModel->GetNodes();

        const glm::vec3 sVec = glm::vec3(1) + (m_axis * (scale / 10));

        for (unsigned int i = 0; i < m_nodeCount; ++i)
        {
            const glm::vec3 t = m_oldPos[i] - m_centre;
            const glm::vec3 sT = t * sVec;
            const glm::vec3 pos = m_centre + sT;

            CurveNodeCluster& c = nodes[m_nodeIndices[i]];
            for (auto iter = c.Nodes.begin(); iter != c.Nodes.end(); ++iter)
            {
                iter->Node.SetPosition(pos);
            }

            for (int j = 0; j < 7; ++j)
            {
                const unsigned int index = m_invNodeIndices[i][j];
                if (index != -1)
                {
                    const e_MirrorMode mode = (e_MirrorMode)(j + 1);
                    const glm::vec3 mul = GetMirrorMultiplier(mode);

                    const glm::vec3 invPos = pos * mul;

                    CurveNodeCluster& c = nodes[index];
                    for (auto iter = c.Nodes.begin(); iter != c.Nodes.end(); ++iter)
                    {
                        iter->Node.SetPosition(invPos);
                    }
                }
            }
        }
    }
    
    m_workspace->PushLongTask(new TriangulateCurveLongTask(m_curveModel));

    return true;
}
bool ScaleCurveNodeAction::Revert()
{
    CurveNodeCluster* nodes = m_curveModel->GetNodes();

    for (unsigned int i = 0; i < m_nodeCount; ++i)
    {
        const glm::vec3& pos = m_oldPos[i];
        CurveNodeCluster& c = nodes[m_nodeIndices[i]];
        for (auto iter = c.Nodes.begin(); iter != c.Nodes.end(); ++iter)
        {
            iter->Node.SetPosition(pos);
        }

        for (int j = 0; j < 7; ++j)
        {
            const unsigned int index = m_invNodeIndices[i][j];
            if (index != -1)
            {
                const e_MirrorMode mode = (e_MirrorMode)(j + 1);
                const glm::vec3 mul = GetMirrorMultiplier(mode);

                const glm::vec3 invPos = pos * mul;

                CurveNodeCluster& c = nodes[index];
                for (auto iter = c.Nodes.begin(); iter != c.Nodes.end(); ++iter)
                {
                    iter->Node.SetPosition(invPos);
                }
            }
        }
    }

    m_workspace->PushLongTask(new TriangulateCurveLongTask(m_curveModel));

    return true;
}