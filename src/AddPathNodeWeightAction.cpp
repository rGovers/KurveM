#include "Actions/AddPathNodeWeightAction.h"

#include "LongTasks/TriangulatePathLongTask.h"
#include "PathModel.h"
#include "Workspace.h"

AddPathNodeWeightAction::AddPathNodeWeightAction(PathModel* a_model, Workspace* a_workspace)
{
    m_workspace = a_workspace;

    m_model = a_model;
}
AddPathNodeWeightAction::~AddPathNodeWeightAction()
{

}

e_ActionType AddPathNodeWeightAction::GetActionType() const
{
    return ActionType_AddPathNodeWeight;
}

void AddPathNodeWeightAction::AddNodeDelta(unsigned int a_index, long long a_bone, float a_delta)
{
    PathNodeCluster* nodes = m_model->GetPathNodes();

    if (nodes[a_index].Nodes.size() > 0)
    {
        float shiftDelta = a_delta;
        const PathNode& node = nodes[a_index].Nodes[0];
        const float weight = node.Node.GetBoneWeight(a_bone);
        const float sum = weight + shiftDelta;

        if (sum > 1)
        {
            shiftDelta -= (sum - 1);
        }
        else if (sum < 0)
        {
            shiftDelta -= sum;
        }

        if (shiftDelta != 0)
        {
            std::vector<PathNode>& nodeData = nodes[a_index].Nodes;
            const unsigned char nodeCount = (unsigned int)nodeData.size();
            for (unsigned char i = 0; i < nodeCount; ++i)
            {
                nodeData[i].Node.SetBoneWeight(a_bone, weight + shiftDelta);
            }

            for (auto iter = m_nodes.begin(); iter != m_nodes.end(); ++iter)
            {
                const PathNodeWeightData data = *iter;
                if (data.Index == a_index && iter->Bone == a_bone)
                {
                    iter->Delta += shiftDelta;

                    return;
                }
            }

            PathNodeWeightData data;

            data.Index = a_index;
            data.Bone = a_bone;
            data.Delta = shiftDelta;

            m_nodes.emplace_back(data);
        }
    }
}

bool AddPathNodeWeightAction::Redo()
{
    PathNodeCluster* nodes = m_model->GetPathNodes();

    for (auto iter = m_nodes.begin(); iter != m_nodes.end(); ++iter)
    {
        const PathNodeWeightData data = *iter;
        std::vector<PathNode>& nodeData = nodes[data.Index].Nodes;
        for (auto innerIter = nodeData.begin(); innerIter != nodeData.end(); ++innerIter)
        {
            const float weight = innerIter->Node.GetBoneWeight(data.Bone);
            innerIter->Node.SetBoneWeight(data.Bone, weight + data.Delta);
        }
    }

    m_workspace->PushLongTask(new TriangulatePathLongTask(m_model));

    return true;
}
bool AddPathNodeWeightAction::Execute()
{
    m_workspace->PushLongTask(new TriangulatePathLongTask(m_model));

    return true;
}
bool AddPathNodeWeightAction::Revert()
{
    PathNodeCluster* nodes = m_model->GetPathNodes();

    for (auto iter = m_nodes.begin(); iter != m_nodes.end(); ++iter)
    {
        const PathNodeWeightData data = *iter;
        std::vector<PathNode>& nodeData = nodes[data.Index].Nodes;
        for (auto innerIter = nodeData.begin(); innerIter != nodeData.end(); ++innerIter)
        {
            const float weight = innerIter->Node.GetBoneWeight(data.Bone);
            innerIter->Node.SetBoneWeight(data.Bone, weight - data.Delta);
        }
    }

    m_workspace->PushLongTask(new TriangulatePathLongTask(m_model));

    return true;
}