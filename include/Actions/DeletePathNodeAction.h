#pragma once

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

#include "Actions/Action.h"

class Editor;
class PathModel;
class Workspace;

struct PathLine;
struct PathNode;
struct PathNodeCluster;

class DeletePathNodeAction : public Action
{
private:
    Workspace*       m_workspace;
    Editor*          m_editor;
  
    PathModel*       m_pathModel;

    unsigned int     m_nodeCount;
    PathNodeCluster* m_nodes;
    unsigned int     m_lineCount;
    PathLine*        m_lines;

    unsigned int     m_oldNodeCount;
    PathNodeCluster* m_oldNodes;
    unsigned int     m_oldLineCount;
    PathLine*        m_oldLines;

    unsigned char GetClusterIndex(const PathNode& a_node, PathNodeCluster* a_newNode);
protected:

public:
    DeletePathNodeAction(Workspace* a_workspace, Editor* a_editor, const unsigned int* a_nodeIndices, unsigned int a_nodeCount, PathModel* a_model);
    virtual ~DeletePathNodeAction();

    virtual e_ActionType GetActionType();

    virtual bool Redo();
    virtual bool Execute();
    virtual bool Revert();
};