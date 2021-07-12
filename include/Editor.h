#pragma once

class Camera;
class Object;
class RenderTexture;
class ShaderProgram;
class Workspace;

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

#include <list>

#include "Actions/Action.h"
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
         
    ShaderProgram*           m_gridShader;

    unsigned int             m_dummyIBO;
    unsigned int             m_dummyVBO;
    unsigned int             m_dummyVAO;

    Camera*                  m_camera;
         
    Workspace*               m_workspace;
         
    RenderTexture*           m_renderTexture;
         
    unsigned char            m_mouseDown;
    glm::vec2                m_startPos;

    std::list<unsigned int>  m_selectedNodes;

    Action*                  m_curAction;

    bool                     m_orthoDown;
    bool                     m_deleteDown;

    bool IsInteractingCurveNode(const glm::vec3& a_pos, const glm::vec3& a_axis, const glm::vec2& a_cursorPos, CurveModel* a_model, const glm::mat4& a_viewProj);
    bool IsInteractingCurveNodeHandle(const Node3Cluster& a_node, unsigned int a_nodeIndex, CurveModel* a_model, const glm::mat4& a_viewProj, const glm::vec2& a_cursorPos, const glm::mat4& a_transform, const glm::vec3& a_up, const glm::vec3& a_right);
    void DrawObject(Object* a_object, const glm::vec2& a_winSize);

    e_ActionType GetCurrentAction() const;

protected:

public:
    Editor(Workspace* a_workspace);
    ~Editor();

    bool IsFaceSelected() const;
    bool CanInsertFace() const;

    inline std::list<unsigned int> GetSelectedNodes() const
    {
        return m_selectedNodes;
    }

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

    inline void ClearSelectedNodes()
    {
        m_selectedNodes.clear();
    }
    void AddNodeToSelection(unsigned int a_index);

    bool IsEditorModeEnabled(e_EditorMode a_editorMode) const;

    void Update(double a_delta, const glm::vec2& a_winPos, const glm::vec2& a_winSize);

    inline RenderTexture* GetRenderTexture() const
    {
        return m_renderTexture;
    }
};