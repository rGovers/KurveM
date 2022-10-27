#include "Windows/PropertiesWindow.h"

#include "ImGuiExt.h"
#include "PropertiesWindows/AnimatePWindow.h"
#include "PropertiesWindows/CurvePWindow.h"
#include "PropertiesWindows/ObjectPWindow.h"
#include "PropertiesWindows/PathPWindow.h"
#include "PropertiesWindows/PhysicsPWindow.h"
#include "PropertiesWindows/RenderingPWindow.h"
#include "Workspace.h"

PropertiesWindow::PropertiesWindow(Workspace* a_workspace, Editor* a_editor)
{
    m_workspace = a_workspace;
    m_editor = a_editor;

    m_propertiesMode = ObjectPropertiesTab_Object;

    m_windows = new PWindow*[ObjectPropertiesTab_End];
    for (int i = 0; i < ObjectPropertiesTab_End; ++i)
    {
        m_windows[i] = nullptr;
    }

    m_windows[ObjectPropertiesTab_Animate] = new AnimatePWindow(a_workspace, a_editor);
    m_windows[ObjectPropertiesTab_Curve] = new CurvePWindow(a_workspace);
    m_windows[ObjectPropertiesTab_Object] = new ObjectPWindow(a_workspace);
    m_windows[ObjectPropertiesTab_Path] = new PathPWindow(a_workspace);
    m_windows[ObjectPropertiesTab_Physics] = new PhysicsPWindow(a_workspace, a_editor);
    m_windows[ObjectPropertiesTab_Rendering] = new RenderingPWindow(a_workspace);
}
PropertiesWindow::~PropertiesWindow()
{
    for (int i = 0; i < ObjectPropertiesTab_End; ++i)
    {
        if (m_windows[i] != nullptr)
        {
            delete m_windows[i];
            m_windows[i] = nullptr;
        }
    }

    delete[] m_windows;
    m_windows = nullptr;
}

void PropertiesWindow::PropertiesTabButton(const char* a_label, const char* a_path, e_ObjectPropertiesTab a_propertiesTab, const char* a_tooltip)
{
    if (ImGuiExt::ImageToggleButton(a_label, a_path, m_propertiesMode == a_propertiesTab, glm::vec2(16.0f)))
    {
        m_propertiesMode = a_propertiesTab;
    }

    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();

        ImGui::Text(a_label);

        ImGui::Separator();

        ImGui::Text(a_tooltip);

        ImGui::EndTooltip();
    }
}

void PropertiesWindow::Update(double a_delta)
{
    if (ImGui::Begin("Properties"))
    {
        const int objectCount = m_workspace->GetSelectedObjectCount();

        if (objectCount > 0)
        {
            Object* obj = m_workspace->GetSelectedObject();
            Transform* transform = obj->GetTransform();

            ImGui::BeginGroup();

            const e_ObjectType objectType = obj->GetObjectType();

            PropertiesTabButton("Object", "Textures/PROPERTIES_OBJECT.png", ObjectPropertiesTab_Object, ObjectTooltip);

            switch (objectType)
            {
            case ObjectType_ArmatureNode:
            {
                if (m_editor->GetEditorMode() == EditorMode_Animate)
                {
                    PropertiesTabButton("Animate", "Textures/PROPERTIES_ANIMATE.png", ObjectPropertiesTab_Animate, AnimateTooltip);
                }

                break;
            }
            case ObjectType_CurveModel:
            {
                PropertiesTabButton("Curve", "Textures/PROPERTIES_CURVE.png", ObjectPropertiesTab_Curve, CurveTooltip);
                PropertiesTabButton("Rendering", "", ObjectPropertiesTab_Rendering, RenderingTooltip);

                break;
            }
            case ObjectType_PathModel:
            {
                PropertiesTabButton("Curve", "Textures/PROPERTIES_PATH.png", ObjectPropertiesTab_Path, PathTooltip);
                PropertiesTabButton("Rendering", "", ObjectPropertiesTab_Rendering, RenderingTooltip);

                break;
            }
            }

            PropertiesTabButton("Physics", "Textures/PROPERTIES_PHYICS.png", ObjectPropertiesTab_Physics, PhysicsTooltip);

            ImGui::EndGroup();

            ImGui::SameLine();

            ImGui::BeginGroup();

            if (m_windows[m_propertiesMode] != nullptr)
            {
                m_windows[m_propertiesMode]->Update();
            }

            ImGui::EndGroup();
        }
    }
    
    ImGui::End();
}