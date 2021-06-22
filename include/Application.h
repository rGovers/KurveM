#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Application
{
private:
    static Application* Instance;

    GLFWwindow* m_window;

    int         m_width;
    int         m_height;

    int         m_xPos;
    int         m_yPos;

protected:
    virtual void Resize(int a_width, int a_height) = 0;
    virtual void Update(double a_delta) = 0;

public:
    Application(int a_width, int a_height, const char* a_title);
    ~Application();

    void Run();

    void Close();

    inline int GetWidth() const
    {
        return m_width;
    }
    inline int GetHeight() const
    {
        return m_height;
    }

    inline int GetXPos() const
    {
        return m_xPos;
    }
    inline int GetYPos() const
    {
        return m_yPos;
    }

    static inline Application* GetInstance() 
    {
        return Instance;
    }

    inline GLFWwindow* GetWindow() const
    {
        return m_window;
    }
};