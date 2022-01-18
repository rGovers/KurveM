#pragma once

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

#include <list>

#include "Actions/Action.h"

struct BezierCurveNode2;

class Camera;
class Editor;
class PathModel;
class RenderTexture;
class ShaderProgram;
class Workspace;

class ShapeEditor
{
private:
    Workspace*              m_workspace;
    Editor*                 m_editor;

    Action*                 m_currentAction;

    RenderTexture*          m_renderTexture;

    Camera*                 m_camera;

    ShaderProgram*          m_gridShader;

    glm::vec2               m_prevCursorPos;

    std::list<unsigned int> m_selectedIndices;

    glm::vec2               m_startCursorPos;

    unsigned char           m_mouseState;

    e_ActionType GetCurrentAction() const;

    void DrawSelection(const BezierCurveNode2& a_node, unsigned int a_index) const;

protected:

public:
    ShapeEditor(Workspace* a_workspace, Editor* a_editor);
    ~ShapeEditor();

    void Init();

    void ClearSelectedNodes();

    void Update(double a_delta, const glm::vec2& a_winPos, const glm::vec2& a_winSize);

    inline RenderTexture* GetRenderTexture() const
    {
        return m_renderTexture;
    }
};