#pragma once

#include "CurveModel.h"

class PrimitiveGenerator
{
private:

protected:

public:
    PrimitiveGenerator() = delete;

    static void CreateCurveSphere(Node3Cluster** a_nodePtr, unsigned int* a_nodeCount, CurveFace** a_facePtr, unsigned int* a_faceCount);
    static void CreateCurveCube(Node3Cluster** a_nodePtr, unsigned int* a_nodeCount, CurveFace** a_facePtr, unsigned int* a_faceCount);
};