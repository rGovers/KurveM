#pragma once

#include "Model.h"

enum e_VertexInputType
{
    VertexInputType_Null = -1,
    VertexInputType_Float,
    VertexInputType_Vec2,
    VertexInputType_Vec3,
    VertexInputType_Vec4,
    VertexInputType_End
};

enum e_VertexInputSlot
{
    VertexInputSlot_Null = -1,
    VertexInputSlot_Position,
    VertexInputSlot_Normal,
    VertexInputSlot_UV,
    VertexInputSlot_Bone,
    VertexInputSlot_Weight,
    VertexInputSlot_SoftbodyIndex,
    VertexInputSlot_SoftbodyWeight,
    VertexInputSlot_End
};

struct VertexInput
{
    unsigned int      Slot;
    e_VertexInputType Type;
    e_VertexInputSlot VertexSlot;

    unsigned int ToOffset() const
    {
        switch (VertexSlot)
        {
        case VertexInputSlot_Position:
        {
            return offsetof(Vertex, Position);
        }
        case VertexInputSlot_Normal:
        {
            return offsetof(Vertex, Normal);
        }
        case VertexInputSlot_UV:
        {
            return offsetof(Vertex, UV);
        }
        case VertexInputSlot_Bone:
        {
            return offsetof(Vertex, Bones);
        }
        case VertexInputSlot_Weight:
        {
            return offsetof(Vertex, Weights);
        }
        case VertexInputSlot_SoftbodyIndex:
        {
            return offsetof(Vertex, BodyI);
        }
        case VertexInputSlot_SoftbodyWeight:
        {
            return offsetof(Vertex, BodyW);
        }
        }

        return 0;
    }
    unsigned int ToCount() const
    {
        switch (Type)
        {
        case VertexInputType_Float:
        {
            return 1;
        }
        case VertexInputType_Vec2:
        {
            return 2;
        }
        case VertexInputType_Vec3:
        {
            return 3;
        }
        case VertexInputType_Vec4:
        {
            return 4;
        }
        }

        return 1;
    }
};