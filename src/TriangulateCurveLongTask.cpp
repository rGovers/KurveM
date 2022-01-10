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
}
TriangulateCurveLongTask::~TriangulateCurveLongTask()
{

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

        return false;
    }

    return true;
}
void TriangulateCurveLongTask::PostExecute()
{
    m_curveModel->PostTriangulate(m_indices, m_indexCount, m_vertices, m_vertexCount);

    delete[] m_indices;
    delete[] m_vertices;
}