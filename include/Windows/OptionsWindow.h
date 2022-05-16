#pragma once

#include "Windows/Window.h"

#include "EditorControls/Editor.h"

class Workspace;

class OptionsWindow : public Window
{
private:
    const char* FrontFaceTooltip = "Displays the object front faces";
    const char* BackFaceTooltip = "Displays the objects back faces";
    const char* BothFaceTooltip = "Displays both of the objects faces";
    const char* NoFaceTooltip = "Displays none of the objects faces";

    const char* MirrorNoneTooltip = "Sets editor mirror mode to none";
    const char* MirrorXTooltip = "Sets editor mirror mode to X";
    const char* MirrorYTooltip = "Sets editor mirror mode to Y";
    const char* MirrorZTooltip = "Sets editor mirror mode to Z";

    const char* EditorModeString[4] = 
    { 
        "Object Mode", 
        "Edit Mode", 
        "Weight Mode",
        "Animate Mode"
    };
    const char* EditorModePath[4] =
    {
        "Textures/EDITOR_OBJECT.png",
        "Textures/EDITOR_EDIT.png",
        "Textures/EDITOR_WEIGHT.png",
        "Textures/EDITOR_ANIMATE.png"
    };
    const char* EditorModePreviewPath[4] =
    {
        "Textures/EDITOR_OBJECT_DARK.png",
        "Textures/EDITOR_EDIT_DARK.png",
        "Textures/EDITOR_WEIGHT_DARK.png",
        "Textures/EDITOR_ANIMATE_DARK.png"
    };

    Workspace* m_workspace;
    Editor*    m_editor;

    void EditorFaceButton(const char* a_text, const char* a_path, e_EditorFaceCullingMode a_face, const char* a_tooltip);
    void EditorMirrorButton(const char* a_text, const char* a_path, e_MirrorMode a_mode, const char* a_tooltip);

    void DeleteAnimation(bool a_state);

protected:

public:
    OptionsWindow(Workspace* a_workspace, Editor* a_editor);
    virtual ~OptionsWindow();

    virtual void Update(double a_delta);
};