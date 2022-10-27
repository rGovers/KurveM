 #include "Actions/SymmetricPathNodeHandleAction.h"

#include "PathModel.h"

SymmetricPathNodeHandleAction::SymmetricPathNodeHandleAction(Workspace* a_workspace, const unsigned int* a_nodeIndices, unsigned int a_nodeCount, PathModel* a_model, e_MirrorMode a_mirrorMode)
{
    m_workspace = a_workspace;

    m_pathModel = a_model;

    m_nodeCount = a_nodeCount;

    const PathNodeCluster* nodes = m_pathModel->GetPathNodes();

    m_nodeIndices = new unsigned int[m_nodeCount];
    m_mirrorIndices = new unsigned int*[m_nodeCount];
    m_handles = new glm::vec3*[m_nodeCount];
    for (unsigned int i = 0; i < m_nodeCount; ++i)
    {
        const unsigned int index = a_nodeIndices[i];

        m_nodeIndices[i] = index;
        m_mirrorIndices[i] = m_pathModel->GetMirroredPathIndices(index, a_mirrorMode);

        const PathNodeCluster& c = nodes[index];
        const unsigned char size = (unsigned char)c.Nodes.size();
        m_handles[i] = new glm::vec3[size];
        for (unsigned char j = 0; j < size; ++j)
        {
            m_handles[i][j] = c.Nodes[j].Node.GetHandlePosition();
        }
    }
}
SymmetricPathNodeHandleAction::~SymmetricPathNodeHandleAction()
{  
    delete[] m_nodeIndices;

    for (unsigned int i = 0; i < m_nodeCount; ++i)
    {
        delete[] m_mirrorIndices[i];
        delete[] m_handles[i];
    }
    delete[] m_mirrorIndices;
    delete[] m_handles;
}

glm::vec3 SymmetricPathNodeHandleAction::GetMirrorMultiplier(e_MirrorMode a_mode) const
{
    glm::vec3 mul = glm::vec3(1.0f);
    if (a_mode & MirrorMode_X)
    {
        mul.x = -1.0f;
    }
    if (a_mode & MirrorMode_Y)
    {
        mul.y = -1.0f;
    }
    if (a_mode & MirrorMode_Z)
    {
        mul.z = -1.0f;
    }

    return mul;
}

e_ActionType SymmetricPathNodeHandleAction::GetActionType() const
{
    return ActionType_SymmetricPathNodeHandle;
}

bool SymmetricPathNodeHandleAction::Redo()
{
    return Execute();
}
bool SymmetricPathNodeHandleAction::Execute()
{
    PathNodeCluster* nodes = m_pathModel->GetPathNodes();

    for (unsigned int i = 0; i < m_nodeCount; ++i)
    {
        PathNodeCluster& c = nodes[i];
        const PathNode& node = c.Nodes[0];

        const glm::vec3 pos = node.Node.GetPosition();
        const glm::vec3 hPos = node.Node.GetHandlePosition();

        const glm::vec3 dir = glm::normalize(hPos - pos);

        const unsigned char size = (unsigned char)c.Nodes.size();
        for (unsigned char j = 0; j < size; ++j)
        {
            PathNode& node = c.Nodes[j];

            const glm::vec3 pos = node.Node.GetPosition();
            const glm::vec3 hPos = node.Node.GetHandlePosition();

            const glm::vec3 nDir = hPos - pos;
            const float len = glm::length(nDir);
            const float sign = glm::sign(glm::dot(dir, nDir));

            node.Node.SetHandlePosition(pos + dir * sign * len);
        }

        for (int j = 0; j < 3; ++j)
        {
            const unsigned int index = m_mirrorIndices[i][j];
            if (index != -1)
            {
                const e_MirrorMode mode = (e_MirrorMode)(j + 1);

                const glm::vec3 mul = GetMirrorMultiplier(mode);
                const glm::vec3 mDir = mul * dir;

                PathNodeCluster& c = nodes[index];
                const unsigned char size = (unsigned char)c.Nodes.size();
                for (unsigned char k = 0; k < size; ++k)
                {
                    PathNode& node = c.Nodes[k];

                    const glm::vec3 pos = node.Node.GetPosition();
                    const glm::vec3 hPos = node.Node.GetHandlePosition();

                    const glm::vec3 nDir = hPos - pos;
                    const float len = glm::length(nDir);
                    const float sign = glm::sign(glm::dot(mDir, nDir));

                    node.Node.SetHandlePosition(pos + mDir * sign * len);
                }
            }
        }
    }

    return true;
}
bool SymmetricPathNodeHandleAction::Revert()
{
    PathNodeCluster* nodes = m_pathModel->GetPathNodes();

    for (unsigned int i = 0; i < m_nodeCount; ++i)
    {
        PathNodeCluster& c = nodes[i];
        const unsigned char size = (unsigned char)c.Nodes.size();
        for (unsigned char j = 0; j < size; ++j)
        {
            c.Nodes[j].Node.SetHandlePosition(m_handles[i][j]);
        }

        for (int j = 0; j < 3; ++j)
        {
            const unsigned int index = m_mirrorIndices[i][j];
            if (index != -1)
            {
                const e_MirrorMode mode = (e_MirrorMode)(j + 1);

                const glm::vec3 mul = GetMirrorMultiplier(mode);

                PathNodeCluster& c = nodes[index];
                const unsigned char size = (unsigned char)c.Nodes.size();
                for (unsigned char k = 0; k < size; ++k)
                {
                    c.Nodes[k].Node.SetHandlePosition(mul * m_handles[i][k]);
                }
            }
        }
    }

    return true;
}