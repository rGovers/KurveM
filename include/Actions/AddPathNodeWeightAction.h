#pragma once

#include "Actions/Action.h"

#include <list>

class PathModel;
class Workspace;

struct PathNodeWeightData 
{
    unsigned int Index;
    long long Bone;
    float Delta;
};

class AddPathNodeWeightAction : public Action
{
private:
    Workspace*                     m_workspace;

    PathModel*                     m_model;

    std::list<PathNodeWeightData> m_nodes;

protected:

public:
    AddPathNodeWeightAction(PathModel* a_model, Workspace* a_workspace);
    virtual ~AddPathNodeWeightAction();

    virtual e_ActionType GetActionType() const;

    void AddNodeDelta(unsigned int a_index, long long a_bone, float a_delta);

    virtual bool Redo();
    virtual bool Execute();
    virtual bool Revert();
};