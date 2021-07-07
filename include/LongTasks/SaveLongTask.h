#pragma once

#include "LongTasks/LongTask.h"

class SaveLongTask : public LongTask
{
private:
    Workspace* m_workspace;

protected:

public:
    SaveLongTask(Workspace* a_workspace);
    virtual ~SaveLongTask();

    virtual bool Execute();
};