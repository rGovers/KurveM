#pragma once

class Modal
{
private:

protected:

public:
    virtual const char* GetName() = 0;

    bool Open();
    virtual bool Execute() = 0;
};