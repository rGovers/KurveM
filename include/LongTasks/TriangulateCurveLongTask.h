#pragma once

#include "LongTasks/LongTask.h"

struct Vertex;

class CurveModel;

class TriangulateCurveLongTask : public LongTask
{
private:
    CurveModel*   m_curveModel;

    unsigned int  m_vertexCount;
    unsigned int  m_indexCount;
 
    Vertex*       m_vertices;
    unsigned int* m_indices;
protected:

public:
    TriangulateCurveLongTask(CurveModel* a_curveModel);
    virtual ~TriangulateCurveLongTask();

    virtual bool PushAction(Workspace* a_workspace);

    virtual bool Execute();
    virtual void PostExecute();
};