#pragma once

enum e_ActionType 
{
    ActionType_Null = -1,
    ActionType_CreateObject,
    ActionType_DeleteNode,
    ActionType_DeleteObject,
    ActionType_ExtrudeNode,
    ActionType_FlipFace,
    ActionType_InsertFace,
    ActionType_MoveNode,
    ActionType_MoveNodeHandle,
    ActionType_RenameObject,
    ActionType_RotateObject,
    ActionType_ScaleObject,
    ActionType_TranslateObject,
    ActionType_TranslateObjectRelative,
    ActionType_End
};

class Action
{
private:

protected:
    
public:

    virtual e_ActionType GetActionType() { return ActionType_Null; } 

    virtual bool Redo() = 0;
    virtual bool Execute() { return true; }
    virtual bool Revert() = 0;
};