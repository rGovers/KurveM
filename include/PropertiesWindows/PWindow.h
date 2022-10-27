#pragma once

#include "Windows/PropertiesWindow.h"

class PWindow
{
private:

protected:

public:
    virtual ~PWindow() { }

    virtual e_ObjectPropertiesTab GetWindowType() const { return ObjectPropertiesTab_Null; }
    
    virtual void Update() = 0;
};