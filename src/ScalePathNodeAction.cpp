#include "Actions/ScalePathNodeAction.h"

#include "LongTasks/TriangulatePathLongTask.h"
#include "PathModel.h"
#include "Workspace.h"

ScalePathNodeAction::ScalePathNodeAction(Workspace* a_workspace, const unsigned int* a_indices, unsigned int a_indexCount, PathModel* a_model, const glm::vec3& a_startPos, const glm::vec3& a_axis)
{   
    m_workspace = a_workspace;

    m_model = a_model;

    m_indexCount = a_indexCount;

    m_startPos = a_startPos;
    m_endPos = m_startPos;

    m_axis = a_axis;

    const PathNode* nodes = m_model->GetNodes();

    m_indices = new unsigned int[m_indexCount];
    m_startScale = new glm::vec2[m_indexCount];

    for (unsigned int i = 0; i < m_indexCount; ++i)
    {
        const unsigned int index = a_indices[i];

        m_indices[i] = index;  
        m_startScale[i] = nodes[index].Scale;
    }
}
ScalePathNodeAction::~ScalePathNodeAction()
{
    delete[] m_startScale;
    delete[] m_indices;
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

        PathNode* nodes = m_model->GetNodes();

        for (unsigned int i = 0; i < m_indexCount; ++i)
        {
            nodes[m_indices[i]].Scale = m_startScale[i] + scaleAxis;
        }

        m_workspace->PushLongTask(new TriangulatePathLongTask(m_model));
    }

    return true;
}
bool ScalePathNodeAction::Revert()
{
    PathNode* nodes = m_model->GetNodes();

    for (unsigned int i = 0; i < m_indexCount; ++i)
    {
        nodes[m_indices[i]].Scale = m_startScale[i];
    }

    m_workspace->PushLongTask(new TriangulatePathLongTask(m_model));

    return true;
}