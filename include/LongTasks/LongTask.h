#pragma once

class Workspace;

class LongTask
{
private:
    char* m_displayName;

protected:

public:
    LongTask(const char* a_displayName);
    virtual ~LongTask();

    inline const char* GetDisplayName() const
    {
        return m_displayName;
    }

    virtual bool PushAction(Workspace* a_workspace) { return true; }

    virtual bool Execute() { return true; }
    virtual void PostExecute() { }
};