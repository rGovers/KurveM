#pragma once

class CurveModel;
class Editor;
class RenderTexture;
class Workspace;

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

#include "Actions/Action.h"

class UVEditor
{
private:
    Workspace*     m_workspace;
    Editor*        m_editor;

    Action*        m_currentAction;

    RenderTexture* m_renderTexture;

    unsigned char  m_mouseDown;

    e_ActionType GetCurrentAction() const;

    void MoveUV(const glm::vec2& a_pos, const glm::vec2& a_cursorPos, const glm::vec2& a_axis, CurveModel* a_curveModel);
protected:

public:
    UVEditor(Workspace* a_workspace, Editor* a_editor);
    ~UVEditor();

    void Update(double a_delta, const glm::vec2& a_winPos, const glm::vec2& a_winSize);

    inline RenderTexture* GetRenderTexture() const
    {
        return m_renderTexture;
    }
};