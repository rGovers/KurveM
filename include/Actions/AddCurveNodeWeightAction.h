#pragma once

#include "Actions/Action.h"

#include <list>

class CurveModel;
class Workspace;

struct CurveNodeWeightData 
{
    unsigned int Index;
    long long Bone;
    float Delta;
};

class AddCurveNodeWeightAction : public Action
{
private:
    Workspace*                     m_workspace;

    CurveModel*                    m_model;

    std::list<CurveNodeWeightData> m_nodes;

protected:

public:
    AddCurveNodeWeightAction(CurveModel* a_model, Workspace* a_workspace);
    virtual ~AddCurveNodeWeightAction();

    virtual e_ActionType GetActionType() const;

    void AddNodeDelta(unsigned int a_index, long long a_bone, float a_delta);

    virtual bool Redo();
    virtual bool Execute();
    virtual bool Revert();
};