#pragma once

#include "LongTasks/LongTask.h"

struct Vertex;

class PathModel;

class TriangulatePathLongTask : public LongTask
{
private:
    PathModel*    m_pathModel;

    unsigned int  m_vertexCount;
    unsigned int  m_indexCount;
 
    Vertex*       m_vertices;
    unsigned int* m_indices;

protected:

public:
    TriangulatePathLongTask(PathModel* a_pathModel);
    virtual ~TriangulatePathLongTask();

    virtual bool PushAction(Workspace* a_workspace);

    virtual bool Execute();
    virtual void PostExecute();
};
