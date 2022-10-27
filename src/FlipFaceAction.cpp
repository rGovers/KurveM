#include "Actions/FlipFaceAction.h"

#include "CurveModel.h"
#include "LongTasks/TriangulateCurveLongTask.h"
#include "Workspace.h"

FlipFaceAction::FlipFaceAction(Workspace* a_workspace, const unsigned int* a_nodeIndices, unsigned int a_nodeCount, CurveModel* a_curveModel)
{
    m_workspace = a_workspace;

    m_nodeCount = a_nodeCount;

    m_model = a_curveModel;

    m_nodeIndices = new unsigned int[m_nodeCount];

    for (unsigned int i = 0; i < m_nodeCount; ++i)
    {
        m_nodeIndices[i] = a_nodeIndices[i];
    }
}

FlipFaceAction::~FlipFaceAction()
{
    delete[] m_nodeIndices;
}

e_ActionType FlipFaceAction::GetActionType() const
{
    return ActionType_FlipFace;
}

void SwapIndices(CurveFace* a_face, e_FaceIndex a_indexA, e_FaceIndex a_indexB)
{
    const unsigned int tVal = a_face->Index[a_indexB];
    const unsigned int tValCluster = a_face->ClusterIndex[a_indexB];

    a_face->Index[a_indexB] = a_face->Index[a_indexA];
    a_face->ClusterIndex[a_indexB] = a_face->ClusterIndex[a_indexA];

    a_face->Index[a_indexA] = tVal;
    a_face->ClusterIndex[a_indexA] = tValCluster;
}

bool FlipFaceAction::Redo()
{
    return Execute();
}
bool FlipFaceAction::Execute()
{
    unsigned int index = -1;

    switch (m_nodeCount)
    {
    case 3:
    {
        index = m_model->Get3PointFaceIndex(m_nodeIndices[0], m_nodeIndices[1], m_nodeIndices[2]);

        break;
    }
    case 6:
    {
        index = m_model->Get3PointFaceIndex(m_nodeIndices);

        break;
    }
    case 4:
    {
        index = m_model->Get4PointFaceIndex(m_nodeIndices[0], m_nodeIndices[1], m_nodeIndices[2], m_nodeIndices[3]);

        break;
    }
    case 8:
    {
        index = m_model->Get4PointFaceIndex(m_nodeIndices);

        break;
    }
    }

    if (index != -1)
    {
        CurveFace* faces = m_model->GetFaces();

        switch (faces[index].FaceMode)
        {
        case FaceMode_3Point:
        {
            SwapIndices(&faces[index], FaceIndex_3Point_AB, FaceIndex_3Point_AC);
            SwapIndices(&faces[index], FaceIndex_3Point_BA, FaceIndex_3Point_CA);
            SwapIndices(&faces[index], FaceIndex_3Point_BC, FaceIndex_3Point_CB);

            break;
        }
        case FaceMode_4Point:
        {
            SwapIndices(&faces[index], FaceIndex_4Point_AB, FaceIndex_4Point_AC);
            SwapIndices(&faces[index], FaceIndex_4Point_BA, FaceIndex_4Point_CA);
            SwapIndices(&faces[index], FaceIndex_4Point_CD, FaceIndex_4Point_BD);
            SwapIndices(&faces[index], FaceIndex_4Point_DB, FaceIndex_4Point_DC);

            break;
        }
        }

        m_workspace->PushLongTask(new TriangulateCurveLongTask(m_model));

        return true;
    }

    return false;
}
bool FlipFaceAction::Revert()
{
    return Execute();
}