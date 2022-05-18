#include "Actions/MoveShapeNodeAction.h"

#include "LongTasks/TriangulatePathLongTask.h"
#include "PathModel.h"
#include "Workspace.h"

MoveShapeNodeAction::MoveShapeNodeAction(Workspace* a_workspace, const unsigned int* a_indices, unsigned int a_indexCount, PathModel* a_pathModel, const glm::vec2& a_pos, const glm::vec2& a_axis, e_MirrorMode a_mirrorMode)
{
    m_workspace = a_workspace;

    m_model = a_pathModel;

    m_startPos = a_pos;
    m_endPos = a_pos;

    m_axis = a_axis;

    const ShapeNodeCluster* nodes = m_model->GetShapeNodes();

    m_indexCount = a_indexCount;

    m_indices = new unsigned int[m_indexCount];
    m_oldPos = new glm::vec2[m_indexCount];
    m_mirrorIndices = new unsigned int*[m_indexCount];

    for (unsigned int i = 0; i < m_indexCount; ++i)
    {
        const unsigned int index = a_indices[i];
        m_indices[i] = index;
        m_oldPos[i] = nodes[index].Nodes[0].GetPosition();
        m_mirrorIndices[i] = m_model->GetMirroredShapeIndices(index, a_mirrorMode);
    }
}
MoveShapeNodeAction::~MoveShapeNodeAction()
{
    delete[] m_indices;
    delete[] m_oldPos;

    for (unsigned int i = 0; i < m_indexCount; ++i)
    {
        delete[] m_mirrorIndices[i];
    }
    delete[] m_mirrorIndices;
}

glm::vec2 MoveShapeNodeAction::GetMirrorMultiplier(e_MirrorMode a_mode) const
{
    glm::vec2 mul = glm::vec3(1.0f);
    if (a_mode & MirrorMode_X)
    {
        mul.x = -1.0f;
    }
    if (a_mode & MirrorMode_Y)
    {
        mul.y = -1.0f;
    }

    return mul;
}

e_ActionType MoveShapeNodeAction::GetActionType()
{
    return ActionType_MoveShapeNode;
}

bool MoveShapeNodeAction::Redo()
{
    return Execute();
}
bool MoveShapeNodeAction::Execute()
{
    ShapeNodeCluster* nodes = m_model->GetShapeNodes();

    const glm::vec2 delta = (m_endPos - m_startPos) * m_axis;

    for (unsigned int i = 0; i < m_indexCount; ++i)
    {
        std::vector<BezierCurveNode2>& cNodes = nodes[m_indices[i]].Nodes;

        const glm::vec2 pos = m_oldPos[i] + delta;

        for (auto iter = cNodes.begin(); iter != cNodes.end(); ++iter)
        {
            iter->SetPosition(pos);
        }

        for (int j = 0; j < 3; ++j)
        {
            const unsigned int index = m_mirrorIndices[i][j];
            if (index != -1)
            {
                const e_MirrorMode mode = (e_MirrorMode)(j + 1);

                const glm::vec2 mul = GetMirrorMultiplier(mode);
                const glm::vec2 mPos = pos * mul;

                std::vector<BezierCurveNode2>& mNode = nodes[index].Nodes;
                for (auto iter = mNode.begin(); iter != mNode.end(); ++iter)
                {
                    iter->SetPosition(mPos);
                }
            }
        }
    }

    m_workspace->PushLongTask(new TriangulatePathLongTask(m_model));

    return true;
}
bool MoveShapeNodeAction::Revert()
{
    ShapeNodeCluster* nodes = m_model->GetShapeNodes();

    for (unsigned int i = 0; i < m_indexCount; ++i)
    {
        std::vector<BezierCurveNode2>& cNodes = nodes[m_indices[i]].Nodes;

        const glm::vec2& pos = m_oldPos[i]; 

        for (auto iter = cNodes.begin(); iter != cNodes.end(); ++iter)
        {
            iter->SetPosition(pos);
        }

        for (int j = 0; j < 3; ++j)
        {
            const unsigned int index = m_mirrorIndices[i][j];
            if (index != -1)
            {
                const e_MirrorMode mode = (e_MirrorMode)(j + 1);

                const glm::vec2 mul = GetMirrorMultiplier(mode);
                const glm::vec2 mPos = pos * mul;

                std::vector<BezierCurveNode2>& mNode = nodes[index].Nodes;
                for (auto iter = mNode.begin(); iter != mNode.end(); ++iter)
                {
                    iter->SetPosition(mPos);
                }
            }
        }
    }

    m_workspace->PushLongTask(new TriangulatePathLongTask(m_model));

    return true;
}