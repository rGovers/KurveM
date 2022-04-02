#pragma once

#include "Application.h"

class Workspace;

class AppMain : public Application
{
private:
    Workspace* m_workspace;

protected:
    virtual void Resize(int a_width, int a_height);
    virtual void Update(double a_delta);

public:
    AppMain();
    virtual ~AppMain();
};