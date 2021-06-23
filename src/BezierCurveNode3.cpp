#include "BezierCurveNode3.h"

BezierCurveNode3::BezierCurveNode3()
{
    m_pos = glm::vec3(0);
    m_handle = glm::vec3(0, 1, 0);
}
BezierCurveNode3::BezierCurveNode3(const glm::vec3& a_pos, const glm::vec3& a_handlePos)
{
    m_pos = a_pos;
    m_handle = a_handlePos;
}
BezierCurveNode3::~BezierCurveNode3()
{

}

glm::vec3 BezierCurveNode3::GetPoint(const BezierCurveNode3& a_other, float a_lerp) const
{
    return BezierCurveNode3::GetPoint(*this, a_other, a_lerp);
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