#include "LongTasks/TriangulatePathLongTask.h"

#include <exception>
#include <stdio.h>

#include "PathModel.h"
#include "Workspace.h"

TriangulatePathLongTask::TriangulatePathLongTask(PathModel* a_pathModel) :
    LongTask("Triangulate Path")
{
    m_pathModel = a_pathModel;

    m_indices = nullptr;
    m_vertices = nullptr;
}
TriangulatePathLongTask::~TriangulatePathLongTask()
{
    if (m_indices != nullptr)
    {
        delete[] m_indices;
        m_indices = nullptr;
    }

    if (m_vertices != nullptr)
    {
        delete[] m_vertices;
        m_vertices = nullptr;
    }
}

bool TriangulatePathLongTask::PushAction(Workspace* a_workspace)
{
    const std::list<LongTask*> tasks = a_workspace->GetTaskQueue();

    for (auto iter = tasks.begin(); iter != tasks.end(); ++iter)
    {
        if (strcmp((*iter)->GetDisplayName(), GetDisplayName()) == 0)
        {
            TriangulatePathLongTask* task = (TriangulatePathLongTask*)*iter;

            if (task->m_pathModel == m_pathModel)
            {
                return false;
            }
        }
    }

    return true;
}

bool TriangulatePathLongTask::Execute()
{
    try
    {
        m_pathModel->PreTriangulate(&m_indices, &m_indexCount, &m_vertices, &m_vertexCount);
    }
    catch(const std::exception& e)
    {
        printf("Error triangulating: ");
        printf(e.what());
        printf("\n");

        if (m_indices != nullptr)
    {
        delete[] m_indices;
        m_indices = nullptr;
    }

    if (m_vertices != nullptr)
    {
        delete[] m_vertices;
        m_vertices = nullptr;
    }

        return false;
    }
    
    return true;
}
void TriangulatePathLongTask::PostExecute()
{
    if (m_indices != nullptr && m_vertices != nullptr)
    {
        m_pathModel->PostTriangulate(m_indices, m_indexCount, m_vertices, m_vertexCount);
    }

    if (m_indices != nullptr)
    {
        delete[] m_indices;
        m_indices = nullptr;
    }

    if (m_vertices != nullptr)
    {
        delete[] m_vertices;
        m_vertices = nullptr;
    }
}