#include "Animation.h"

#include <string.h>

Animation::Animation(const char* a_name, Workspace* a_workspace)
{
    m_workspace = a_workspace;

    const int strLength = strlen(a_name) + 1;

    m_name = new char[strLength];

    for (int i = 0; i < strLength; ++i)
    {
        m_name[i] = a_name[i];
    }

    m_referenceFramerate = 24;
    m_length = 1.0f;
}
Animation::~Animation()
{
    delete[] m_name;
}