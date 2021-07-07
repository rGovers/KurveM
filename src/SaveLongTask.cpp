#include "LongTasks/SaveLongTask.h"

#include <exception>
#include <stdio.h>

#include "Workspace.h"

SaveLongTask::SaveLongTask(Workspace* a_workspace) :
    LongTask("Save File")
{
    m_workspace = a_workspace;
}
SaveLongTask::~SaveLongTask()
{

}

bool SaveLongTask::Execute()
{
    try
    {
        m_workspace->Save();
    }
    catch (std::exception e)
    {
        printf("Error saving: ");
        printf(e.what());
        printf("\n");

        return false;
    }

    return true;
}