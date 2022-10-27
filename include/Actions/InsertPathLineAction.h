#pragma once

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

#include "Actions/Action.h"

class PathModel;
class Workspace;

struct PathNodeCluster;

class InsertPathLineAction : public Action
{
private:
    Workspace*    m_workspace;

    PathModel*    m_pathModel;

    unsigned int  m_nodeCount;
    unsigned int* m_nodeIndices;
    unsigned int  m_lineIndex;

    unsigned char PushNode(PathNodeCluster* a_cluster);

protected:

public:
    InsertPathLineAction(Workspace* a_workspace, const unsigned int* a_nodeIndices, unsigned int a_nodeCount, PathModel* a_model);
    virtual ~InsertPathLineAction();

    virtual e_ActionType GetActionType() const;

    virtual bool Redo();
    virtual bool Execute();
    virtual bool Revert();
};