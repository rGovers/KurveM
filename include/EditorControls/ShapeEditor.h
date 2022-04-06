#pragma once

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

#include <list>

#include "Actions/Action.h"
#include "AxisControl.h"

struct ShapeNodeCluster;

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

    float                   m_cameraZoom;

    e_ActionType GetCurrentAction() const;

    void DrawSelection(const ShapeNodeCluster& a_node, unsigned int a_index) const;

    bool MoveNode(const glm::mat4& a_viewProj, const glm::vec2& a_pos, const glm::vec2& a_cursorPos, e_Axis a_axis, PathModel* a_pathModel);
    bool MoveNodeHandle(const glm::mat4& a_viewProj, const ShapeNodeCluster& a_node, unsigned int a_index, const glm::vec2& a_cursorPos, PathModel* a_pathModel);

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