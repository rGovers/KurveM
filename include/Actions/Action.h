#pragma once

enum e_ActionType 
{
    ActionType_Null = -1,
    ActionType_AddAnimationNode,
    ActionType_AddCurveNodeWeight,
    ActionType_AddPathNodeWeight,
    ActionType_CreateAnimation,
    ActionType_CreateObject,
    ActionType_DeleteAnimation,
    ActionType_DeleteCurveNode,
    ActionType_DeleteObject,
    ActionType_DeletePathNode,
    ActionType_ExtrudeArmatureNode,
    ActionType_ExtrudeCurveNode,
    ActionType_ExtrudePathNode,
    ActionType_ExtrudeShapeNode,
    ActionType_FlipFace,
    ActionType_InsertFace,
    ActionType_InsertPathLine,
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
    ActionType_SetBoxCollisionShapeHalfExtents,
    ActionType_SetCapsuleCollisionShapeHeight,
    ActionType_SetCapsuleCollisionShapeRadius,
    ActionType_SetSphereCollisionShapeRadius,
    ActionType_SetCollisionObjectActive,
    ActionType_SetCollisionObjectType,
    ActionType_SetCollisionShapeType,
    ActionType_SetCurveArmature,
    ActionType_SetCurveSmartStep,
    ActionType_SetCurveSteps,
    ActionType_SetPathArmature,
    ActionType_SetPathPathSteps,
    ActionType_SetPathShapeSteps,
    ActionType_SetPlaneCollisionShapeDirection,
    ActionType_SetPlaneCollisionShapeDistance,
    ActionType_SetSoftbodyDampening,
    ActionType_SetSoftbodyFaceAngularStiffness,
    ActionType_SetSoftbodyFaceStiffness,
    ActionType_SetSoftbodyFaceVolumeStiffness,
    ActionType_SetSoftbodyLineAngularStiffness,
    ActionType_SetSoftbodyLineStiffness,
    ActionType_SetSoftbodyLineVolumeStiffness,
    ActionType_SetRigidbodyMass,
    ActionType_SetSoftbodyMass,
    ActionType_SymmetricPathNodeHandle,
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

    virtual void SetData(void* a_data) { } 

    virtual bool Redo() = 0;
    virtual bool Execute() { return true; }
    virtual bool Revert() = 0;
};