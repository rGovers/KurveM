#pragma once

class Action
{
private:

protected:

public:

    virtual bool Redo() = 0;
    virtual bool Execute() { return true; }
    virtual bool Revert() = 0;
};