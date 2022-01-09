#include "BezierCurveNode2.h"

BezierCurveNode2::BezierCurveNode2() :
    BezierCurveNode2(glm::vec2(0), glm::vec2(0))
{
}
BezierCurveNode2::BezierCurveNode2(const glm::vec2& a_pos, const glm::vec2& a_handlePos)
{
    m_pos = a_pos;
    m_handle = a_handlePos;
}
BezierCurveNode2::~BezierCurveNode2()
{

}

glm::vec2 BezierCurveNode2::GetPoint(const BezierCurveNode2& a_other, float a_lerp) const
{
    return GetPoint(*this, a_other, a_lerp);
}
glm::vec2 BezierCurveNode2::GetPointScaled(const BezierCurveNode2& a_other, float a_scale, float a_lerp) const
{
    return GetPointScaled(*this, a_other, a_scale, a_lerp);
}

glm::vec2 BezierCurveNode2::GetPoint(const BezierCurveNode2& a_pointA, const BezierCurveNode2& a_pointB, float a_lerp)
{
    const glm::vec2 stepA = glm::mix(a_pointA.m_pos, a_pointA.m_handle, a_lerp);
    const glm::vec2 stepB = glm::mix(a_pointA.m_handle, a_pointB.m_handle, a_lerp);
    const glm::vec2 stepC = glm::mix(a_pointB.m_handle, a_pointB.m_pos, a_lerp);

    const glm::vec2 innerStepA = glm::mix(stepA, stepB, a_lerp);
    const glm::vec2 innerStepB = glm::mix(stepB, stepC, a_lerp);

    return glm::mix(innerStepA, innerStepB, a_lerp); 
}
glm::vec2 BezierCurveNode2::GetPointScaled(const BezierCurveNode2& a_pointA, const BezierCurveNode2& a_pointB, float a_scale, float a_lerp)
{
    const glm::vec2 pHADiff = a_pointA.m_handle - a_pointA.m_pos;
    const glm::vec2 pHBDiff = a_pointB.m_handle - a_pointB.m_pos;

    const glm::vec2 pAHandle = a_pointA.m_pos + (pHADiff * a_scale);
    const glm::vec2 pBHandle = a_pointB.m_pos + (pHBDiff * a_scale);

    const glm::vec2 stepA = glm::mix(a_pointA.m_pos, pAHandle, a_lerp);
    const glm::vec2 stepB = glm::mix(pAHandle, pBHandle, a_lerp);
    const glm::vec2 stepC = glm::mix(pBHandle, a_pointB.m_pos, a_lerp);

    const glm::vec2 innerStepA = glm::mix(stepA, stepB, a_lerp);
    const glm::vec2 innerStepB = glm::mix(stepB, stepC, a_lerp);

    return glm::mix(innerStepA, innerStepB, a_lerp);
}