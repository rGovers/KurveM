#include "LongTasks/LongTask.h"

#include <string.h>

LongTask::LongTask(const char* a_displayName)
{
    const unsigned int len = strlen(a_displayName) + 1;

    m_displayName = new char[len];

    for (unsigned int i = 0; i < len; ++i)
    {
        m_displayName[i] = a_displayName[i];
    }   
}
LongTask::~LongTask()
{
    delete[] m_displayName;
}