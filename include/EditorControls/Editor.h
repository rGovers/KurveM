#pragma once

class Camera;
class CameraController;
class ControlEditor;
class EditorInputController;
class Object;
class PhysicsEngine;
class RenderTexture;
class ShaderProgram;
class Workspace;

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

#include <list>

#include "Actions/Action.h"

enum e_EditorMode
{
    EditorMode_Object,
    EditorMode_Edit,
    EditorMode_WeightPainting,
    EditorMode_Animate,
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
         
    PhysicsEngine*            m_engine;

    RenderTexture*            m_renderTexture;
         
    unsigned char             m_mouseDown;
    glm::vec2                 m_startPos;

    std::list<ControlEditor*> m_editorControls;

    std::list<unsigned int>   m_selectedNodes;
    std::list<long long>      m_selectedArmNodes;

    long long                 m_selectedWeightNode;

    Action*                   m_curAction;

    float                     m_brushIntensity;
    float                     m_brushRadius;

    float                     m_animTime;
    float                     m_selectedTime;

    void DrawObject(Object* a_object, const glm::vec2& a_winSize);

protected:

public:
    Editor(Workspace* a_workspace);
    ~Editor();

    inline PhysicsEngine* GetPhysicsEngine() const
    {
        return m_engine;
    }

    e_ActionType GetCurrentActionType() const;
    inline Action* GetCurrentAction() const
    {
        return m_curAction;
    }
    inline void SetCurrentAction(Action* a_action)
    {
        m_curAction = a_action;
    }

    inline float GetBrushRadius() const
    {
        return m_brushRadius;
    }
    inline void SetBrushRadius(float a_value)
    {
        m_brushRadius = a_value;
    }

    inline float GetBrushIntensity() const
    {
        return m_brushIntensity;
    }
    inline void SetBrushIntensity(float a_value)
    {
        m_brushIntensity = a_value;
    }

    inline float GetAnimationTime() const
    {
        return m_animTime;
    }
    inline void SetAnimationTime(float a_time)
    {
        m_animTime = a_time;
    }

    inline float GetSelectedTime() const
    {
        return m_selectedTime;
    }
    inline void SetSelectedTime(float a_time)
    {
        m_selectedTime = a_time;
    }

    void Init();

    inline long long GetSelectedWeightNode() const
    {
        return m_selectedWeightNode;
    }
    void SetSelectedWeightNode(const Object* a_value);
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
    void SetEditorMode(e_EditorMode a_editorMode);

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

    void PathDeleteDown(Object* a_object);
    void PathFDown(Object* a_object);

    void Update(double a_delta, const glm::vec2& a_winPos, const glm::vec2& a_winSize);
    void UpdateContextMenu(const glm::vec2& a_winPos, const glm::vec2& a_winSize);

    inline RenderTexture* GetRenderTexture() const
    {
        return m_renderTexture;
    }
};