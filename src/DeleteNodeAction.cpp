#include "Actions/DeleteNodeAction.h"

#include <vector>
#include <unordered_map>

#include "Editor.h"
#include "LongTasks/TriangulateCurveLongTask.h"
#include "Workspace.h"

struct DeleteClusterNode
{
    unsigned int Index;
    unsigned int ClusterIndex;

    BezierCurveNode3 Node;
};

unsigned int GetClusterIndex(const BezierCurveNode3& a_node, Node3Cluster* a_newNode)
{
    if (a_newNode->Nodes[0].Node.GetHandlePosition().x == std::numeric_limits<float>::infinity())
    {
        a_newNode->Nodes[0].Node.SetHandlePosition(a_node.GetHandlePosition());
        ++a_newNode->Nodes[0].FaceCount;

        return 0;
    }

    const unsigned int size = a_newNode->Nodes.size();
    for (unsigned int i = 0; i < size; ++i)
    {
        if (a_node.GetHandlePosition() == a_newNode->Nodes[i].Node.GetHandlePosition())
        {
            ++a_newNode->Nodes[i].FaceCount; 

            return i;      
        }
    }

    a_newNode->Nodes.emplace_back(a_node);

    return size;
}

DeleteNodeAction::DeleteNodeAction(Workspace* a_workspace, Editor* a_editor, const unsigned int* a_nodeIndices, unsigned int a_nodeCount, CurveModel* a_curveModel)
{
    m_workspace = a_workspace;
    m_editor = a_editor;

    m_curveModel = a_curveModel;

    m_own = false;

    m_oldNodes = m_curveModel->GetNodes();
    m_oldFaces = m_curveModel->GetFaces();

    m_oldNodeCount = m_curveModel->GetNodeCount();
    m_oldFaceCount = m_curveModel->GetFaceCount();

    std::unordered_map<unsigned int, unsigned int> lookup;

    m_nodes = new Node3Cluster[m_oldNodeCount];

    m_nodeCount = 0;
    for (unsigned int i = 0; i < m_oldNodeCount; ++i)
    {
        bool found = false;
        for (int j = 0; j < a_nodeCount; ++j)
        {
            if (a_nodeIndices[j] == i)
            {
                found = true;

                break;
            }
        }

        if (!found)
        {
            NodeGroup nGroup = m_oldNodes[i].Nodes[0];
            nGroup.FaceCount = 0;
            nGroup.Node.SetHandlePosition(glm::vec3(std::numeric_limits<float>::infinity()));

            m_nodes[m_nodeCount] = nGroup;

            lookup.emplace(i, m_nodeCount++);
        }
    }

    m_faces = new CurveFace[m_oldFaceCount];

    m_faceCount = 0;
    for (unsigned int i = 0; i < m_oldFaceCount; ++i)
    {
        const CurveFace face = m_oldFaces[i];
        const int iterCount = face.FaceMode == FaceMode_3Point ? 6 : 8;

        bool found = true;
        for (int j = 0; j < iterCount; ++j)
        {
            if (lookup.find(face.Index[j]) == lookup.end())
            {
                found = false;

                break;
            }
        }

        if (found)
        {
            CurveFace newFace;

            newFace.FaceMode = face.FaceMode;

            for (int j = 0; j < iterCount; ++j)
            {
                const unsigned int index = face.Index[j];
                const unsigned int newIndex = lookup[index];

                newFace.Index[j] = newIndex;
                newFace.ClusterIndex[j] = GetClusterIndex(m_oldNodes[index].Nodes[face.ClusterIndex[j]].Node, &m_nodes[newIndex]);
            }

            m_faces[m_faceCount++] = newFace;
        }
    }
}
DeleteNodeAction::~DeleteNodeAction()
{
    if (m_own)
    {
        delete[] m_oldNodes;
        delete[] m_oldFaces;
    }
    else
    {
        delete[] m_nodes;
        delete[] m_faces;
    }
}

e_ActionType DeleteNodeAction::GetActionType()
{
    return ActionType_DeleteNode;
}

bool DeleteNodeAction::Redo()
{
    return Execute();
}
bool DeleteNodeAction::Execute()
{
    if (m_nodeCount != m_oldNodeCount || m_faceCount != m_oldFaceCount)
    {
        m_editor->ClearSelectedNodes();

        m_own = true;

        m_curveModel->PassModelData(m_nodes, m_nodeCount, m_faces, m_faceCount);

        m_workspace->PushLongTask(new TriangulateCurveLongTask(m_curveModel));

        return true;
    }

    return false;
}
bool DeleteNodeAction::Revert()
{
    if (m_nodeCount != m_oldNodeCount || m_faceCount != m_oldFaceCount)
    {
        m_editor->ClearSelectedNodes();

        m_own = false;

        m_curveModel->PassModelData(m_oldNodes, m_oldNodeCount, m_oldFaces, m_oldFaceCount);

        m_workspace->PushLongTask(new TriangulateCurveLongTask(m_curveModel));

        return true;
    }

    return false;
}