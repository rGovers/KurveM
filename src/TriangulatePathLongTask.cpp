#include "LongTasks/TriangulatePathLongTask.h"

#include <exception>
#include <stdio.h>

#include "PathModel.h"
#include "Workspace.h"

TriangulatePathLongTask::TriangulatePathLongTask(PathModel* a_pathModel) :
    LongTask("Triangulate Path")
{
    m_pathModel = a_pathModel;
}
TriangulatePathLongTask::~TriangulatePathLongTask()
{

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

        return false;
    }
    
    return true;
}
void TriangulatePathLongTask::PostExecute()
{
    m_pathModel->PostTriangulate(m_indices, m_indexCount, m_vertices, m_vertexCount);
}