#include "AppMain.h"

#include <glad/glad.h>
#include <string>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"
#include "KurveMConfig.h"
#include "Workspace.h"

void GLAPIENTRY
MessageCallback( GLenum a_source,
                 GLenum a_type,
                 GLuint a_id,
                 GLenum a_severity,
                 GLsizei a_length,
                 const GLchar* a_message,
                 const void* a_userParam )
{
    printf("GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
           ( a_type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
            a_type, a_severity, a_message );
}

void GUIStyle()
{
    ImGuiStyle* style = &ImGui::GetStyle();
    
    style->WindowRounding = 0.0f;
    style->TabRounding = 4.0f;
    style->FrameRounding = 10.0f;
    style->GrabRounding = 12.0f;

    style->WindowBorderSize = 0.0f;
    style->ChildBorderSize = 0.0f;
    style->PopupBorderSize = 0.0f;

    style->WindowMenuButtonPosition = ImGuiDir_Right;

    style->WindowTitleAlign = { 0.5f, 0.5f };

    ImVec4* colors = style->Colors;

    colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg]               = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_ChildBg]                = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_PopupBg]                = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
    colors[ImGuiCol_Border]                 = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg]                = ImVec4(0.93f, 0.53f, 0.00f, 1.00f);
    colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.93f, 0.53f, 0.00f, 1.00f);
    colors[ImGuiCol_FrameBgActive]          = ImVec4(0.61f, 0.35f, 0.02f, 1.00f);
    colors[ImGuiCol_TitleBg]                = ImVec4(0.42f, 0.42f, 0.42f, 1.00f);
    colors[ImGuiCol_TitleBgActive]          = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    colors[ImGuiCol_MenuBarBg]              = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
    colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    colors[ImGuiCol_CheckMark]              = ImVec4(0.61f, 0.35f, 0.02f, 1.00f);
    colors[ImGuiCol_SliderGrab]             = ImVec4(0.61f, 0.35f, 0.02f, 1.00f);
    colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.93f, 0.53f, 0.00f, 1.00f);
    colors[ImGuiCol_Button]                 = ImVec4(0.93f, 0.53f, 0.00f, 1.00f);
    colors[ImGuiCol_ButtonHovered]          = ImVec4(0.97f, 0.60f, 0.12f, 0.80f);
    colors[ImGuiCol_ButtonActive]           = ImVec4(0.61f, 0.35f, 0.00f, 1.00f);
    colors[ImGuiCol_Header]                 = ImVec4(0.93f, 0.53f, 0.00f, 1.00f);
    colors[ImGuiCol_HeaderHovered]          = ImVec4(0.97f, 0.60f, 0.12f, 0.80f);
    colors[ImGuiCol_HeaderActive]           = ImVec4(0.97f, 0.60f, 0.12f, 1.00f);
    colors[ImGuiCol_Separator]              = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.97f, 0.60f, 0.12f, 0.78f);
    colors[ImGuiCol_SeparatorActive]        = ImVec4(0.97f, 0.60f, 0.12f, 0.78f);
    colors[ImGuiCol_ResizeGrip]             = ImVec4(0.93f, 0.53f, 0.00f, 0.20f);
    colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.97f, 0.60f, 0.12f, 0.67f);
    colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.61f, 0.35f, 0.02f, 0.95f);
    colors[ImGuiCol_Tab]                    = ImVec4(0.61f, 0.35f, 0.02f, 1.00f);
    colors[ImGuiCol_TabHovered]             = ImVec4(0.97f, 0.60f, 0.12f, 0.80f);
    colors[ImGuiCol_TabActive]              = ImVec4(0.93f, 0.53f, 0.00f, 1.00f);
    colors[ImGuiCol_TabUnfocused]           = ImVec4(0.61f, 0.35f, 0.02f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.93f, 0.53f, 0.00f, 1.00f);
    colors[ImGuiCol_DockingPreview]         = ImVec4(0.93f, 0.53f, 0.00f, 0.70f);
    colors[ImGuiCol_DockingEmptyBg]         = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_PlotLines]              = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
    colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
    colors[ImGuiCol_TableBorderLight]       = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
    colors[ImGuiCol_TableRowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt]          = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
    colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.61f, 0.35f, 0.02f, 1.00f);
    colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}

AppMain::AppMain() :
    Application(1280, 720, ("KurveM " + std::to_string(KURVEM_VERSION_MAJOR) + "." + std::to_string(KURVEM_VERSION_MINOR)).c_str())
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    io.ConfigWindowsMoveFromTitleBarOnly = true;

    // Get errors on windows using OpenGL in release mode
#if DEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);
#endif

    ImGui::StyleColorsDark();
    GUIStyle();

    if (!ImGui_ImplGlfw_InitForOpenGL(GetWindow(), true))
	{
		assert(0);
	}
	if (!ImGui_ImplOpenGL3_Init("#version 130"))
	{
		assert(0);
	}

    m_workspace = new Workspace();
}
AppMain::~AppMain()
{
    delete m_workspace;

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void AppMain::Resize(int a_width, int a_height)
{
    m_workspace->Resize(a_width, a_height);
}
void AppMain::Update(double a_delta)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGuiIO& io = ImGui::GetIO();

    glClearColor(0.3, 0.3, 0.3, 1);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);    

    m_workspace->Update(a_delta);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
}