#include "Actions/ScalePathNodeAction.h"

#include "LongTasks/TriangulatePathLongTask.h"
#include "PathModel.h"
#include "Workspace.h"

ScalePathNodeAction::ScalePathNodeAction(Workspace* a_workspace, const unsigned int* a_indices, unsigned int a_indexCount, PathModel* a_model, const glm::vec3& a_startPos, const glm::vec3& a_axis, e_MirrorMode a_mirrorMode)
{   
    m_workspace = a_workspace;

    m_model = a_model;

    m_indexCount = a_indexCount;

    m_startPos = a_startPos;
    m_endPos = m_startPos;

    m_axis = a_axis;

    const PathNodeCluster* nodes = m_model->GetPathNodes();

    m_indices = new unsigned int[m_indexCount];
    m_startScale = new glm::vec2[m_indexCount];
    m_mirrorIndices = new unsigned int*[m_indexCount];

    for (unsigned int i = 0; i < m_indexCount; ++i)
    {
        const unsigned int index = a_indices[i];

        m_indices[i] = index;  
        m_startScale[i] = nodes[index].Nodes[0].Scale;
        m_mirrorIndices[i] = m_model->GetMirroredPathIndices(index, a_mirrorMode);
    }
}
ScalePathNodeAction::~ScalePathNodeAction()
{
    delete[] m_startScale;
    delete[] m_indices;

    for (unsigned int i = 0; i < m_indexCount; ++i)
    {
        delete[] m_mirrorIndices[i];
    }
    delete[] m_mirrorIndices;
}

e_ActionType ScalePathNodeAction::GetActionType()
{
    return ActionType_ScalePathNode;
}

bool ScalePathNodeAction::Redo()
{
    return Execute();
}
bool ScalePathNodeAction::Execute()
{
    const glm::vec3 endAxis = m_endPos - m_startPos;
    const float len = glm::length(endAxis);

    if (len > 0)
    {
        const glm::vec3 scaledAxis = m_axis * len;
        const float scale = glm::dot(scaledAxis, endAxis);

        const glm::vec2 scaleAxis = m_axis.xz() * scale;

        PathNodeCluster* nodes = m_model->GetPathNodes();

        for (unsigned int i = 0; i < m_indexCount; ++i)
        {
            const glm::vec2 scale = m_startScale[i] + scaleAxis;

            PathNodeCluster& c = nodes[m_indices[i]];
            for (auto iter = c.Nodes.begin(); iter != c.Nodes.end(); ++iter)
            {
                iter->Scale = scale;
            }

            for (int j = 0; j < 7; ++j)
            {
                const unsigned int index = m_mirrorIndices[i][j];
                if (index != -1)
                {
                    PathNodeCluster& c = nodes[index];
                    for (auto iter = c.Nodes.begin(); iter != c.Nodes.end(); ++iter)
                    {
                        iter->Scale = scale;
                    }
                }
            }
        }

        m_workspace->PushLongTask(new TriangulatePathLongTask(m_model));
    }

    return true;
}
bool ScalePathNodeAction::Revert()
{
    PathNodeCluster* nodes = m_model->GetPathNodes();

    for (unsigned int i = 0; i < m_indexCount; ++i)
    {
        const glm::vec2& scale = m_startScale[i];

        PathNodeCluster& c = nodes[m_indices[i]];
        for (auto iter = c.Nodes.begin(); iter != c.Nodes.end(); ++iter)
        {
            iter->Scale = scale;
        }

        for (int j = 0; j < 7; ++j)
        {
            const unsigned int index = m_mirrorIndices[i][j];
            if (index != -1)
            {
                PathNodeCluster& c = nodes[index];
                for (auto iter = c.Nodes.begin(); iter != c.Nodes.end(); ++iter)
                {
                    iter->Scale = scale;
                }
            }
        }
    }

    m_workspace->PushLongTask(new TriangulatePathLongTask(m_model));

    return true;
}