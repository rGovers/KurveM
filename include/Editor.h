#pragma once

class Camera;
class CameraController;
class ControlEditor;
class EditorInputController;
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
    EditorMode_WeightPainting,
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
    EditorInputController*    m_inputController;

    e_EditorFaceCullingMode   m_faceCullingMode;
    e_EditorMode              m_editorMode;
         
    ShaderProgram*            m_gridShader;

    Camera*                   m_camera;
    CameraController*         m_cameraController;
         
    Workspace*                m_workspace;
         
    RenderTexture*            m_renderTexture;
         
    unsigned char             m_mouseDown;
    glm::vec2                 m_startPos;

    std::list<ControlEditor*> m_editorControls;

    std::list<unsigned int>   m_selectedNodes;
    std::list<long long>      m_selectedArmNodes;

    long long                 m_selectedWeightNode;

    Action*                   m_curAction;

    void DrawObject(Object* a_object, const glm::vec2& a_winSize);

protected:

public:
    Editor(Workspace* a_workspace);
    ~Editor();

    e_ActionType GetCurrentActionType() const;
    inline Action* GetCurrentAction() const
    {
        return m_curAction;
    }
    inline void SetCurrentAction(Action* a_action)
    {
        m_curAction = a_action;
    }

    void Init();

    bool IsFaceSelected() const;
    bool CanInsertFace() const;

    inline long long GetSelectedWeightNode() const
    {
        return m_selectedWeightNode;
    }
    void SetSelectedWeightNode(Object* a_value);
    inline void SetSelectedWeightNode(long long a_value)
    {
        m_selectedWeightNode = a_value;
    }

    inline std::list<long long> GetSelectedArmatureNodes() const
    {
        return m_selectedArmNodes;
    }
    long long* GetSelectedArmatureNodesArray() const;
    inline int GetSelectedArmatureNodesCount() const
    {
        return m_selectedArmNodes.size();
    }
    Object** GetSelectedArmatureObjectsArray() const;
    int GetSelectedArmatureObjectsCount() const;

    inline std::list<unsigned int> GetSelectedNodes() const
    {
        return m_selectedNodes;
    }
    unsigned int* GetSelectedNodesArray() const;
    inline unsigned int GetSelectedNodeCount() const
    {
        return m_selectedNodes.size();
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

    void ClearSelectedNodes();
    
    void AddNodeToSelection(unsigned int a_index);
    void RemoveNodeFromSelection(unsigned int a_index);

    void AddArmatureNodeToSelection(long long a_id);
    void AddArmatureNodeToSelection(Object* a_object);
    void RemoveArmatureNodeFromSelection(long long a_id);
    void RemoveArmatureNodeFromSelection(Object* a_object);

    bool IsEditorModeEnabled(e_EditorMode a_editorMode) const;

    void CurveDeleteDown(Object* a_object);
    void CurveFDown(Object* a_object);
    void CurveNDown(Object* a_object);

    void Update(double a_delta, const glm::vec2& a_winPos, const glm::vec2& a_winSize);

    inline RenderTexture* GetRenderTexture() const
    {
        return m_renderTexture;
    }
};