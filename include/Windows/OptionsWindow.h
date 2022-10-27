#pragma once

#include "Windows/Window.h"

#include "EditorControls/Editor.h"

class Workspace;

class OptionsWindow : public Window
{
private:
    static constexpr char* MirrorXTooltip = "Sets editor mirror mode to X";
    static constexpr char* MirrorYTooltip = "Sets editor mirror mode to Y";
    static constexpr char* MirrorZTooltip = "Sets editor mirror mode to Z";

    static constexpr char* DisplayFaceName[EditorFaceCullingMode_End] = 
    {
        "Front Faces",
        "Back Faces",
        "Both Faces",
        "No Faces"
    };
    static constexpr char* DisplayFaceTooltip[EditorFaceCullingMode_End] = 
    {
        "Displays the object front faces",
        "Displays the objects back faces",
        "Displays both of the objects faces",
        "Displays none of the objects faces"
    };
    static constexpr char* DisplayFacePaths[EditorFaceCullingMode_End] = 
    {
        "Textures/CULL_BACK.png",
        "Textures/CULL_FRONT.png",
        "Textures/CULL_NONE.png",
        "Textures/CULL_ALL.png"
    };

    static constexpr char* DrawName[EditorDrawMode_End] =
    {
        "Shaded",
        "Solid",
        "Wireframe",
        "Custom"
    };
    static constexpr char* DrawTooltip[EditorDrawMode_End] = 
    {
        "Shaded view of the scene",
        "Solid view of the scene",
        "Wireframe view of the scene",
        "Display the scene with custom shaders"
    };

    static constexpr char* EditorModeString[EditorMode_End] = 
    { 
        "Object Mode", 
        "Edit Mode", 
        "Weight Mode",
        "Animate Mode"
    };
    static constexpr char* EditorModePath[EditorMode_End] =
    {
        "Textures/EDITOR_OBJECT.png",
        "Textures/EDITOR_EDIT.png",
        "Textures/EDITOR_WEIGHT.png",
        "Textures/EDITOR_ANIMATE.png"
    };
    static constexpr char* EditorModePreviewPath[EditorMode_End] =
    {
        "Textures/EDITOR_OBJECT_DARK.png",
        "Textures/EDITOR_EDIT_DARK.png",
        "Textures/EDITOR_WEIGHT_DARK.png",
        "Textures/EDITOR_ANIMATE_DARK.png"
    };

    Workspace* m_workspace;
    Editor*    m_editor;

    void EditorFaceButton(e_EditorFaceCullingMode a_face);
    void EditorDrawButton(e_EditorDrawMode a_mode);

    void EditorMirrorButton(const char* a_text, const char* a_path, e_MirrorMode a_mode, const char* a_tooltip);

    void DeleteAnimation(bool a_state);

protected:

public:
    OptionsWindow(Workspace* a_workspace, Editor* a_editor);
    virtual ~OptionsWindow();

    virtual void Update(double a_delta);
};