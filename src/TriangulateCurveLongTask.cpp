#include "LongTasks/TriangulateCurveLongTask.h"

#include <exception>
#include <stdio.h>
#include <string.h>

#include "CurveModel.h"
#include "Model.h"
#include "Workspace.h"

TriangulateCurveLongTask::TriangulateCurveLongTask(CurveModel* a_curveModel) : 
    LongTask("Triangulate Curve")
{
    m_curveModel = a_curveModel;

    m_indices = nullptr;
    m_vertices = nullptr;
}
TriangulateCurveLongTask::~TriangulateCurveLongTask()
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

bool TriangulateCurveLongTask::PushAction(Workspace* a_workspace)
{
    const std::list<LongTask*> tasks = a_workspace->GetTaskQueue();

    for (auto iter = tasks.begin(); iter != tasks.end(); ++iter)
    {
        if (strcmp((*iter)->GetDisplayName(), GetDisplayName()) == 0)
        {
            TriangulateCurveLongTask* task = (TriangulateCurveLongTask*)*iter;

            if (task->m_curveModel == m_curveModel)
            {
                return false;
            }
        }
    } 

    return true;
}

bool TriangulateCurveLongTask::Execute()
{
    try
    {
        m_curveModel->PreTriangulate(&m_indices, &m_indexCount, &m_vertices, &m_vertexCount);
    }
    catch (std::exception e)
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
void TriangulateCurveLongTask::PostExecute()
{
    if (m_indices != nullptr && m_vertices != nullptr)
    {
        m_curveModel->PostTriangulate(m_indices, m_indexCount, m_vertices, m_vertexCount);
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