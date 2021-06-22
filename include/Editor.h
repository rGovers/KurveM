#pragma once

class Camera;
class Object;
class RenderTexture;
class Workspace;

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

#include <list>

#include "CurveModel.h"

enum e_EditorMode
{
    EditorMode_Object,
    EditorMode_Edit,
    EditorMode_End
};

enum e_EditorFaceCullingMode
{
    EditorFaceCullingMode_Back,
    EditorFaceCullingMode_Front,
    EditorFaceCullingMode_None,
    EditorFaceCullingMode_All,
    EditorFaceCullingMode_End
};

class Editor
{
private:
    e_EditorFaceCullingMode  m_faceCullingMode;
    e_EditorMode             m_editorMode;
         
    Camera*                  m_camera;
         
    Workspace*               m_workspace;
         
    RenderTexture*           m_renderTexture;
         
    unsigned char            m_mouseDown;
    glm::vec2                m_startPos;

    std::list<Node3Cluster*> m_selectedNodes;

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

    inline e_EditorFaceCullingMode GetEditorFaceCullingMode() const
    {
        return m_faceCullingMode;
    }
    inline void SetEditorFaceCullingMode(e_EditorFaceCullingMode a_cullingMode)
    {
        m_faceCullingMode = a_cullingMode;
    }

    bool IsEditorModeEnabled(e_EditorMode a_editorMode) const;

    void Update(double a_delta, const glm::vec2& a_winPos, const glm::vec2& a_winSize);

    inline RenderTexture* GetRenderTexture() const
    {
        return m_renderTexture;
    }
};