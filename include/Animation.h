#pragma once

class Workspace;

class Animation
{
private:
    Workspace* m_workspace;

    char*      m_name;

    int        m_referenceFramerate;
    float      m_length;

protected:

public:
    Animation(const char* a_name, Workspace* a_workspace);
    ~Animation();

    inline const char* GetName() const
    {
        return m_name;
    }

    inline int GetReferenceFramerate() const
    {
        return m_referenceFramerate;
    }
    void SetReferenceFramerate(int a_value)
    {
        m_referenceFramerate = a_value;
    }

    inline float GetAnimationLength() const
    {
        return m_length;
    }
    inline void SetAnimationLength(float a_value)
    {
        m_length = a_value;
    }
};
