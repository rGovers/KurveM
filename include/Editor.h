#pragma once

class Camera;
class Object;
class RenderTexture;
class Workspace;

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

enum e_EditorMode
{
    EditorMode_Object,
    EditorMode_Edit,
    EditorMode_End
};

class Editor
{
private:
    e_EditorMode   m_editorMode;

    Camera*        m_camera;

    Workspace*     m_workspace;

    RenderTexture* m_renderTexture;

    bool           m_mouseDown;

    void DrawObject(Object* a_object, const glm::vec2& a_winSize);

protected:

public:
    Editor(Workspace* a_workspace);
    ~Editor();

    inline e_EditorMode GetEditorMode() const
    {
        return m_editorMode;
    }
    inline void SetEditorMode(e_EditorMode a_editorMode)
    {
        m_editorMode = a_editorMode;
    }

    bool IsEditorModeEnabled(e_EditorMode a_editorMode) const;

    void Update(double a_delta, const glm::vec2& a_winPos, const glm::vec2& a_winSize);

    inline RenderTexture* GetRenderTexture() const
    {
        return m_renderTexture;
    }
};