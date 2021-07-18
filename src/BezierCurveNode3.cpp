#include "BezierCurveNode3.h"

#include "Workspace.h"

BezierCurveNode3::BezierCurveNode3()
{
    m_pos = glm::vec3(0);
    m_handle = glm::vec3(0, 1, 0);

    m_uv = glm::vec2(0);
}
BezierCurveNode3::BezierCurveNode3(const glm::vec3& a_pos, const glm::vec3& a_handlePos, const glm::vec2& a_uv)
{
    m_pos = a_pos;
    m_handle = a_handlePos;

    m_uv = a_uv;
}
BezierCurveNode3::~BezierCurveNode3()
{

}

BoneCluster* BezierCurveNode3::GetBonesLerp(const BezierCurveNode3& a_other, float a_lerp, unsigned int* a_count) const
{
    return GetBonesLerp(*this, a_other, a_lerp, a_count);
}

glm::vec2 BezierCurveNode3::GetUVLerp(const BezierCurveNode3& a_other, float a_lerp) const
{
    return GetUVLerp(*this, a_other, a_lerp);
}

glm::vec3 BezierCurveNode3::GetPoint(const BezierCurveNode3& a_other, float a_lerp) const
{
    return GetPoint(*this, a_other, a_lerp);
}
glm::vec3 BezierCurveNode3::GetPointScaled(const BezierCurveNode3& a_other, float a_scale, float a_lerp) const
{
    return GetPointScaled(*this, a_other, a_scale, a_lerp);
}

BoneCluster* BezierCurveNode3::GetBonesLerp(const BezierCurveNode3& a_pointA, const BezierCurveNode3& a_pointB, float a_lerp, unsigned int* a_count)
{
    const unsigned int sizeA = a_pointA.m_bones.size();
    const unsigned int sizeB = a_pointB.m_bones.size();

    const unsigned int size = sizeA + sizeB;

    const float invLerp = 1 - a_lerp;

    BoneCluster* cluster = new BoneCluster[size]; 

    const std::vector<BoneCluster> arrA = a_pointA.m_bones;
    std::vector<BoneCluster> arrB = a_pointB.m_bones;

    *a_count = 0;
    for (auto iter = arrA.begin(); iter != arrA.end(); ++iter)
    {
        bool found = false;
        for (auto innerIter = arrB.begin(); innerIter != arrB.end(); ++innerIter)
        {
            cluster[*a_count].ID = iter->ID;

            if (iter->ID == innerIter->ID)
            {
                found = true;

                cluster[*a_count].Weight = glm::mix(iter->Weight, innerIter->Weight, a_lerp);

                arrB.erase(innerIter);

                break;
            }
        }

        if (!found)
        {
            cluster[*a_count].Weight = glm::mix(iter->Weight, 0.0f, a_lerp);
        }

        ++*a_count;
    }

    for (auto iter = arrB.begin(); iter != arrB.end(); ++iter)
    {
        cluster[*a_count].ID = iter->ID;
        cluster[*a_count].Weight = glm::mix(iter->Weight, 0.0f, invLerp);

        ++*a_count;
    }

    return cluster;
}

glm::vec2 BezierCurveNode3::GetUVLerp(const BezierCurveNode3& a_pointA, const BezierCurveNode3& a_pointB, float a_lerp)
{
    return glm::mix(a_pointA.m_uv, a_pointB.m_uv, a_lerp);
}

glm::vec3 BezierCurveNode3::GetPoint(const BezierCurveNode3& a_pointA, const BezierCurveNode3& a_pointB, float a_lerp)
{
    // I am confused by complex maths so I stared at the graph till it made sense
    // There is probably someone that can understand the equation and do it better but that is not me
    const glm::vec3 stepA = glm::mix(a_pointA.m_pos, a_pointA.m_handle, a_lerp);
    const glm::vec3 stepB = glm::mix(a_pointA.m_handle, a_pointB.m_handle, a_lerp);
    const glm::vec3 stepC = glm::mix(a_pointB.m_handle, a_pointB.m_pos, a_lerp);

    const glm::vec3 innerStepA = glm::mix(stepA, stepB, a_lerp);
    const glm::vec3 innerStepB = glm::mix(stepB, stepC, a_lerp);

    return glm::mix(innerStepA, innerStepB, a_lerp);
}
glm::vec3 BezierCurveNode3::GetPointScaled(const BezierCurveNode3& a_pointA, const BezierCurveNode3& a_pointB, float a_scale, float a_lerp)
{
    const glm::vec3 pHADiff = a_pointA.m_handle - a_pointA.m_pos;
    const glm::vec3 pHBDiff = a_pointB.m_handle - a_pointB.m_pos;
    
    const glm::vec3 pAHandle = a_pointA.m_pos + (pHADiff * a_scale);
    const glm::vec3 pBHandle = a_pointB.m_pos + (pHBDiff * a_scale);

    const glm::vec3 stepA = glm::mix(a_pointA.m_pos, pAHandle, a_lerp);
    const glm::vec3 stepB = glm::mix(pAHandle, pBHandle, a_lerp);
    const glm::vec3 stepC = glm::mix(pBHandle, a_pointB.m_pos, a_lerp);

    const glm::vec3 innerStepA = glm::mix(stepA, stepB, a_lerp);
    const glm::vec3 innerStepB = glm::mix(stepB, stepC, a_lerp);

    return glm::mix(innerStepA, innerStepB, a_lerp);
}