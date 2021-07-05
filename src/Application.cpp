#include "Application.h"

#include <assert.h>
#include <stdio.h>

Application* Application::Instance = nullptr;

void ErrorCallback(int a_error, const char* a_description)
{
    printf("Error: %s \n", a_description);
}

Application::Application(int a_width, int a_height, const char* a_title)
{
    m_width = a_width;
    m_height = a_height;

    if (!glfwInit())
    {
        assert(0);
    }

    glfwSetErrorCallback(ErrorCallback);
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    m_window = glfwCreateWindow(a_width, a_height, a_title, NULL, NULL);
    if (!m_window)
    {
        glfwTerminate();
        assert(0);
    }
    glfwMakeContextCurrent(m_window);
    // glfwMaximizeWindow(m_window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        assert(0);
    }

    glfwSwapInterval(1);

    glfwGetWindowPos(m_window, &m_xPos, &m_yPos);

    if (Instance == nullptr)
    {
        Instance = this;
    }
}
Application::~Application()
{
    glfwDestroyWindow(m_window);

    glfwTerminate();

    if (Instance == this)
    {
        Instance = nullptr;
    }
}

void Application::Run()
{
    double prevTime = glfwGetTime();

    while (!glfwWindowShouldClose(m_window))
    {
        // ImGui Multiviewport screws with context so I reset it each frame
        glfwMakeContextCurrent(m_window);

        double time = glfwGetTime();

        int width = 0;
        int height = 0;

        glfwGetWindowPos(m_window, &m_xPos, &m_yPos);
        glfwGetWindowSize(m_window, &width, &height);
        if (width != m_width || height != m_height)
        {
            Resize(width, height);

            m_width = width;
            m_height = height;    
        }
        glViewport(0, 0, m_width, m_height);

        glfwPollEvents();

        Update(time - prevTime);

        glfwSwapBuffers(m_window);

        prevTime = time;
    }
}

void Application::Close()
{
    glfwSetWindowShouldClose(m_window, true);
}