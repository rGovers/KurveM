#include "Actions/AddCurveNodeWeightAction.h"

#include "CurveModel.h"
#include "LongTasks/TriangulateCurveLongTask.h"
#include "Workspace.h"

AddCurveNodeWeightAction::AddCurveNodeWeightAction(CurveModel* a_model, Workspace* a_workspace)
{
    m_workspace = a_workspace;

    m_model = a_model;
}
AddCurveNodeWeightAction::~AddCurveNodeWeightAction()
{

}

e_ActionType AddCurveNodeWeightAction::GetActionType()
{
    return ActionType_AddCurveNodeWeight;
}

void AddCurveNodeWeightAction::AddNodeDelta(unsigned int a_index, long long a_bone, float a_delta)
{
    CurveNodeCluster* nodes = m_model->GetNodes();

    if (nodes[a_index].Nodes.size() > 0)
    {
        float shiftDelta = a_delta;
        const CurveNode& node = nodes[a_index].Nodes[0];
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
            const unsigned int nodeGroupCount = nodes[a_index].Nodes.size();
            for (unsigned int i = 0; i < nodeGroupCount; ++i)
            {
                nodes[a_index].Nodes[i].Node.SetBoneWeight(a_bone, weight + shiftDelta);
            }

            for (auto iter = m_nodes.begin(); iter != m_nodes.end(); ++iter)
            {
                if (iter->Index == a_index && iter->Bone == a_bone)
                {
                    iter->Delta += shiftDelta;

                    return;
                }
            }

            CurveNodeWeightData dat;

            dat.Index = a_index;
            dat.Bone = a_bone;
            dat.Delta = shiftDelta;

            m_nodes.emplace_back(dat);
        }
    }
}

bool AddCurveNodeWeightAction::Redo()
{
    CurveNodeCluster* nodes = m_model->GetNodes();

    for (auto iter = m_nodes.begin(); iter != m_nodes.end(); ++iter)
    {
        for (auto innerIter = nodes[iter->Index].Nodes.begin(); innerIter != nodes[iter->Index].Nodes.end(); ++innerIter)
        {
            const float weight = innerIter->Node.GetBoneWeight(iter->Bone);
            innerIter->Node.SetBoneWeight(iter->Bone, weight + iter->Delta);
        }
    }

    m_workspace->PushLongTask(new TriangulateCurveLongTask(m_model));

    return true;
}
bool AddCurveNodeWeightAction::Execute()
{
    m_workspace->PushLongTask(new TriangulateCurveLongTask(m_model));

    return true;
}
bool AddCurveNodeWeightAction::Revert()
{
    CurveNodeCluster* nodes = m_model->GetNodes();

    for (auto iter = m_nodes.begin(); iter != m_nodes.end(); ++iter)
    {
        for (auto innerIter = nodes[iter->Index].Nodes.begin(); innerIter != nodes[iter->Index].Nodes.end(); ++innerIter)
        {
            const float weight = innerIter->Node.GetBoneWeight(iter->Bone);
            innerIter->Node.SetBoneWeight(iter->Bone, weight - iter->Delta);
        }
    }

    m_workspace->PushLongTask(new TriangulateCurveLongTask(m_model));

    return true;
}