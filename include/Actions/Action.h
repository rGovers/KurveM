#pragma once

enum e_ActionType 
{
    ActionType_Null = -1,
    ActionType_AddAnimationNode,
    ActionType_AddCurveNodeWeight,
    ActionType_CreateAnimation,
    ActionType_CreateObject,
    ActionType_DeleteAnimation,
    ActionType_DeleteNode,
    ActionType_DeleteObject,
    ActionType_ExtrudeArmatureNode,
    ActionType_ExtrudeCurveNode,
    ActionType_FlipFace,
    ActionType_InsertFace,
    ActionType_MoveCurveNode,
    ActionType_MoveCurveNodeHandle,
    ActionType_MovePathNode,
    ActionType_MovePathNodeHandle,
    ActionType_MoveShapeNode,
    ActionType_MoveShapeNodeHandle,
    ActionType_MoveUVAction,
    ActionType_RemoveAnimationNode,
    ActionType_RotateCurveNode,
    ActionType_RenameObject,
    ActionType_RotateObject,
    ActionType_RotateObjectRelative,
    ActionType_RotatePathNode,
    ActionType_ScaleCurveNode,
    ActionType_ScaleObject,
    ActionType_ScaleObjectRelative,
    ActionType_ScalePathNode,
    ActionType_SetAnimationFramerate,
    ActionType_SetAnimationLength,
    ActionType_SetAnimationNode,
    ActionType_SetCurveArmature,
    ActionType_SetCurveSmartStep,
    ActionType_SetCurveSteps,
    ActionType_SetPathPathSteps,
    ActionType_SetPathShapeSteps,
    ActionType_TranslateObject,
    ActionType_TranslateObjectRelative,
    ActionType_End
};

class Action
{
private:

protected:
    
public:
    virtual ~Action() { }

    virtual e_ActionType GetActionType() { return ActionType_Null; } 

    virtual bool Redo() = 0;
    virtual bool Execute() { return true; }
    virtual bool Revert() = 0;
};