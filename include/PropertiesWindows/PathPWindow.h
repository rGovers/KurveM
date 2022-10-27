#pragma once

#include "PropertiesWindows/PWindow.h"

#include <vector>

#include "Windows/PropertiesWindow.h"

class Object;
class Workspace;

class PathPWindow : public PWindow
{
private:
    Workspace*     m_workspace;

    int GetArmatures(std::vector<Object*>* a_items, long long a_id, Object* a_object) const;
protected:

public:
    PathPWindow(Workspace* a_workspace);
    virtual ~PathPWindow();

    virtual e_ObjectPropertiesTab GetWindowType() const;
    virtual void Update();
};