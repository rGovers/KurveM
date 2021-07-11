#pragma once

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

class Modal
{
private:

protected:

public:
    virtual ~Modal() { }

    virtual const char* GetName() = 0;

    virtual glm::vec2 GetSize() { return glm::vec2(0); } 

    bool Open();
    virtual bool Execute() = 0;
};