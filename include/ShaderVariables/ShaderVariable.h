#pragma once

#include <list>

class SVar;

enum e_VariableType
{
    VariableType_Null = -1,
    VariableType_Mat3,
    VariableType_Mat4,
    VariableType_End
};

enum e_VariableInputType
{
    VariableInputType_Null = -1,
    VariableInputType_Value,
    VariableInputType_CameraView,
    VariableInputType_CameraProj,
    VariableInputType_Transform,
    VariableInputType_End
};

struct ShaderVariable
{
    unsigned int Slot;
    e_VariableType Type;
    e_VariableInputType InputType;
    void* Variable;

    inline std::list<e_VariableType> GetVariableTypes() const
    {
        std::list<e_VariableType> types;

        switch (InputType)
        {
        case VariableInputType_CameraView:
        {
            types.emplace_back(VariableType_Mat3);
            types.emplace_back(VariableType_Mat4);

            break;
        }
        case VariableInputType_Value:
        {
            types.emplace_back(VariableType_Mat3);
            types.emplace_back(VariableType_Mat4);

            break;
        }
        case VariableInputType_Transform:
        {
            types.emplace_back(VariableType_Mat3);
            types.emplace_back(VariableType_Mat4);

            break;
        }
        }

        return types;
    }
};
