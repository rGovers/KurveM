#include "PropertiesWindows/RenderingPWindow.h"

#include "Actions/AddShaderVariableAction.h"
#include "Actions/AddVertexInputAction.h"
#include "Actions/SetPixelShaderAction.h"
#include "Actions/SetShaderVariableInputTypeAction.h"
#include "Actions/SetShaderVariableSlotAction.h"
#include "Actions/SetShaderVariableTypeAction.h"
#include "Actions/SetShaderVariableValueAction.h"
#include "Actions/SetVertexInputSlotAction.h"
#include "Actions/SetVertexInputTypeAction.h"
#include "Actions/SetVertexInputVertexSlotAction.h"
#include "Actions/SetVertexShaderAction.h"
#include "Datastore.h"
#include "Modals/LoadShaderModal.h"
#include "Object.h"
#include "ShaderProgram.h"
#include "ShaderSettings.h"
#include "ShaderVariables/ShaderVariable.h"
#include "ShaderVariables/VertexInput.h"
#include "Workspace.h"

RenderingPWindow::RenderingPWindow(Workspace* a_workspace)
{
    m_workspace = a_workspace;
}
RenderingPWindow::~RenderingPWindow()
{

}

e_ObjectPropertiesTab RenderingPWindow::GetWindowType() const
{
    return ObjectPropertiesTab_Rendering;   
}

void RenderingPWindow::Update()
{
    const Object* obj = m_workspace->GetSelectedObject();
    if (obj != nullptr)
    {
        ShaderSettings* settings = obj->GetShaderSettings();
        if (settings != nullptr)
        {
            Object* const* objs = m_workspace->GetSelectedObjectArray();
            const unsigned int objectCount = m_workspace->GetSelectedObjectCount();

            const char* vertPath = settings->GetVertexShaderPath();
            if (vertPath == nullptr)
            {
                vertPath = ShaderProgram::BaseShaderName;
            }

            if (ImGui::BeginCombo("Vertex Shader", vertPath))
            {
                if (ImGui::Selectable(ShaderProgram::BaseShaderName))
                {
                    m_workspace->PushActionSet(new SetVertexShaderAction(m_workspace, objs, objectCount, nullptr), "Error setting vertex shader");
                }

                const std::list<std::string> pathList = Datastore::GetVertexPathList();

                for (std::string iter : pathList)
                {
                    const char* str = iter.c_str();

                    if (ImGui::Selectable(str))
                    {
                        m_workspace->PushActionSet(new SetVertexShaderAction(m_workspace, objs, objectCount, str), "Error setting vertex shader");
                    }
                }

                ImGui::Separator();

                if (ImGui::Selectable("Import Shader"))
                {
                    const char* path = m_workspace->GetHomePath();

                    m_workspace->PushModal(new LoadShaderModal(m_workspace, path, objs, objectCount, ShaderLoadMode_Vertex));

                    delete[] path;
                }

                ImGui::EndCombo();
            }

            if (vertPath != ShaderProgram::BaseShaderName)
            {
                ImGui::Text("Vertex Inputs");
                if (ImGui::Button("Add Vertex Input"))
                {
                    m_workspace->PushActionSet(new AddVertexInputAction(objs, objectCount), "Error adding vertex input");
                }

                ImGui::Indent();

                const unsigned int vInputSize = settings->GetVertexInputSize();
                for (unsigned int i = 0; i < vInputSize; ++i)
                {
                    VertexInput* input = settings->GetVertexInput(i);
                    if (input != nullptr)
                    {
                        ImGui::Separator();
                        
                        const std::string str = "##VInput[" + std::to_string(i) + "]";

                        int slot = (int)input->Slot;
                        ImGui::PushID((str + "Slot").c_str());
                        if (ImGui::InputInt("Slot", &slot))
                        {
                            if (slot < -1)
                            {
                                slot = -1;
                            }

                            switch (m_workspace->GetCurrentActionType())
                            {
                            case ActionType_SetVertexInputSlot:
                            {
                                SetVertexInputSlotAction* action = (SetVertexInputSlotAction*)m_workspace->GetCurrentAction();
                                if (action->GetIndex() == i)
                                {
                                    action->SetSlot((unsigned int)slot);
                                    action->Execute();

                                    break;
                                }
                            }
                            default:
                            {
                                m_workspace->PushActionSet(new SetVertexInputSlotAction(objs, objectCount, i, (unsigned int)slot), "Error setting vertex input slot");

                                break;
                            }
                            }
                        }
                        ImGui::PopID();

                        const e_VertexInputType type = input->Type;
                        ImGui::PushID((str + "Type").c_str());
                        if (ImGui::BeginCombo("Type", VertexInputTypeString[type + 1]))
                        {
                            for (int j = 0; j < VertexInputType_End; ++j)
                            {
                                const char* str = VertexInputTypeString[j + 1];

                                const bool selected = j == type;

                                if (ImGui::Selectable(str, selected))
                                {
                                    switch (m_workspace->GetCurrentActionType())
                                    {
                                    case ActionType_SetVertexInputType:
                                    {
                                        SetVertexInputTypeAction* action = (SetVertexInputTypeAction*)m_workspace->GetCurrentAction();
                                        if (action->GetIndex() == i)
                                        {
                                            action->SetType((e_VertexInputType)j);
                                            action->Execute();

                                            break;
                                        }
                                    }
                                    default:
                                    {
                                        m_workspace->PushActionSet(new SetVertexInputTypeAction(objs, objectCount, i, (e_VertexInputType)j), "Error setting vertex input type");

                                        break;
                                    }
                                    }
                                }

                                if (selected)
                                {
                                    ImGui::SetItemDefaultFocus();
                                }
                            }

                            ImGui::EndCombo();
                        }
                        ImGui::PopID();

                        const e_VertexInputSlot vertexSlot = input->VertexSlot;
                        ImGui::PushID((str + "VSlot").c_str());
                        if (ImGui::BeginCombo("Vertex Slot", VertexInputSlotString[vertexSlot + 1]))
                        {
                            for (int j = 0; j < VertexInputSlot_End; ++j)
                            {
                                const char* str = VertexInputSlotString[j + 1];

                                const bool selected = j == vertexSlot;

                                if (ImGui::Selectable(str, selected))
                                {
                                    switch (m_workspace->GetCurrentActionType())
                                    {
                                    case ActionType_SetVertexInputVertexSlot:
                                    {
                                        SetVertexInputVertexSlotAction* action = (SetVertexInputVertexSlotAction*)m_workspace->GetCurrentAction();
                                        if (action->GetIndex() == i)
                                        {
                                            action->SetSlot((e_VertexInputSlot)j);
                                            action->Execute();

                                            break;
                                        }
                                    }
                                    default:
                                    {
                                        m_workspace->PushActionSet(new SetVertexInputVertexSlotAction(objs, objectCount, i, (e_VertexInputSlot)j), "Error setting vertex input vertex slot");

                                        break;
                                    }
                                    }
                                }

                                if (selected)
                                {
                                    ImGui::SetItemDefaultFocus();
                                }
                            }

                            ImGui::EndCombo();
                        }
                        ImGui::PopID();
                    }
                }

                ImGui::Separator();
                ImGui::Unindent();
            }

            const char* pixelPath = settings->GetPixelShaderPath();
            if (pixelPath == nullptr)
            {
                pixelPath = ShaderProgram::BaseShaderName;
            }

            ImGui::Separator();

            if (ImGui::BeginCombo("Pixel Shader", pixelPath))
            {
                if (ImGui::Selectable(ShaderProgram::BaseShaderName))
                {
                    m_workspace->PushActionSet(new SetPixelShaderAction(m_workspace, objs, objectCount, nullptr), "Error setting pixel shader");
                }

                const std::list<std::string> pathList = Datastore::GetPixelPathList();

                for (std::string iter : pathList)
                {
                    const char* str = iter.c_str();

                    if (ImGui::Selectable(str))
                    {
                        m_workspace->PushActionSet(new SetPixelShaderAction(m_workspace, objs, objectCount, str), "Error setting pixel shader");
                    }
                }

                ImGui::Separator();

                if (ImGui::Selectable("Import Shader"))
                {
                    const char* path = m_workspace->GetHomePath();

                    m_workspace->PushModal(new LoadShaderModal(m_workspace, path, objs, objectCount, ShaderLoadMode_Pixel));

                    delete[] path;
                }

                ImGui::EndCombo();
            }

            if (vertPath != ShaderProgram::BaseShaderName || pixelPath != ShaderProgram::BaseShaderName)
            {
                ImGui::Separator();
                
                ImGui::Text("Shader Variables");

                if (ImGui::Button("Add Shader Variable"))
                {
                    m_workspace->PushActionSet(new AddShaderVariableAction(objs, objectCount), "Error adding shader variable");
                }

                ImGui::Indent();
                const unsigned int variableCount = settings->GetShaderVariableSize();
                for (unsigned int i = 0; i < variableCount; ++i)
                {
                    ImGui::Separator();

                    ShaderVariable* var = settings->GetShaderVariable(i);

                    const std::string str = "##SVar[" + std::to_string(i) + "]";

                    int slot = (int)var->Slot;
                    ImGui::PushID((str + "Slot").c_str());
                    if (ImGui::InputInt("Slot", &slot))
                    {
                        if (slot < -1)
                        {
                            slot = -1;
                        }

                        switch (m_workspace->GetCurrentActionType())
                        {
                        case ActionType_SetShaderVariableSlot:
                        {
                            SetShaderVariableSlotAction* action = (SetShaderVariableSlotAction*)m_workspace->GetCurrentAction();
                            if (action->GetIndex() == i)
                            {
                                action->SetSlot((unsigned int)slot);
                                action->Execute();

                                break;
                            }
                        }
                        default:
                        {
                            m_workspace->PushActionSet(new SetShaderVariableSlotAction(objs, objectCount, i, (unsigned int)slot), "Error setting shader variable slot");

                            break;
                        }
                        }
                    }
                    ImGui::PopID();

                    e_VariableInputType inputType = var->InputType;
                    ImGui::PushID((str + "InType").c_str());
                    if (ImGui::BeginCombo("Input Type", ShaderInputTypeInputString[inputType + 1]))
                    {
                        for (int j = 0; j < VariableInputType_End; ++j)
                        {
                            const char* str = ShaderInputTypeInputString[j + 1];

                            const bool selected = j == inputType;

                            if (ImGui::Selectable(str, selected))
                            {
                                switch (m_workspace->GetCurrentActionType())
                                {
                                case ActionType_SetShaderVariableInputType:
                                {
                                    SetShaderVariableInputTypeAction* action = (SetShaderVariableInputTypeAction*)m_workspace->GetCurrentAction();
                                    if (action->GetIndex() == i)
                                    {
                                        action->SetType((e_VariableInputType)j);
                                        action->Execute();

                                        break;
                                    }
                                }
                                default:
                                {
                                    m_workspace->PushActionSet(new SetShaderVariableInputTypeAction(objs, objectCount, i, (e_VariableInputType)j), "Error setting shader variable input type");

                                    break;
                                }
                                }
                            }

                            if (selected)
                            {
                                ImGui::SetItemDefaultFocus();
                            }
                        }

                        ImGui::EndCombo();
                    }
                    ImGui::PopID();

                    const std::list<e_VariableType> allowedTypes = var->GetVariableTypes();
                    if (allowedTypes.size() > 0)
                    {
                        e_VariableType type = var->Type;
                        ImGui::PushID((str + "SType").c_str());
                        if (ImGui::BeginCombo("Type", ShaderInputTypeString[type + 1]))
                        {
                            for (const e_VariableType vType : allowedTypes)
                            {
                                const char* str = ShaderInputTypeString[vType + 1];

                                const bool selected = vType == type;

                                if (ImGui::Selectable(str, selected))
                                {
                                    switch (m_workspace->GetCurrentActionType())
                                    {
                                    case ActionType_SetShaderVariableType:
                                    {
                                        SetShaderVariableTypeAction* action = (SetShaderVariableTypeAction*)m_workspace->GetCurrentAction();
                                        if (action->GetIndex() == i)
                                        {
                                            action->SetType(vType);
                                            action->Execute();

                                            break;
                                        }
                                    }
                                    default:
                                    {
                                        m_workspace->PushActionSet(new SetShaderVariableTypeAction(objs, objectCount, i, vType), "Error settings shader variable type");

                                        break;
                                    }
                                    }
                                }

                                if (selected)
                                {
                                    ImGui::SetItemDefaultFocus();
                                }
                            }

                            ImGui::EndCombo();
                        }
                        ImGui::PopID();

                        if (inputType == VariableInputType_Value)
                        {
                            switch (type)
                            {
                            case VariableType_Mat3:
                            {
                                glm::mat3 mat = *(glm::mat3*)var->Variable;
                                bool set = false;

                                for (int i = 0; i < 3; ++i)
                                {
                                    const std::string valStr = "V[" + std::to_string(i) + "]";

                                    glm::vec3 val = mat[i];

                                    ImGui::PushID((str + valStr).c_str());
                                    if (ImGui::InputFloat3(valStr.c_str(), (float*)&val))
                                    {
                                        mat[i] = val;
                                        set = true;
                                    }
                                    ImGui::PopID();
                                }

                                if (set)
                                {
                                    switch (m_workspace->GetCurrentActionType())
                                    {
                                    case ActionType_SetShaderVariableValue:
                                    {
                                        SetShaderVariableValueAction* action = (SetShaderVariableValueAction*)m_workspace->GetCurrentAction();
                                        if (action->GetIndex() == i)
                                        {
                                            action->SetData(&mat);
                                            action->Execute();

                                            break;
                                        }
                                    }
                                    default:
                                    {
                                        m_workspace->PushActionSet(new SetShaderVariableValueAction(objs, objectCount, i, VariableType_Mat3, &mat), "Error setting shader value mat3");

                                        break;
                                    }
                                    }
                                }

                                break;
                            }
                            case VariableType_Mat4:
                            {
                                glm::mat4 mat = *(glm::mat4*)var->Variable;
                                bool set = false;

                                for (int i = 0; i < 4; ++i)
                                {
                                    const std::string valStr = "V[" + std::to_string(i) + "]";

                                    glm::vec4 val = mat[i];

                                    ImGui::PushID((str + valStr).c_str());
                                    if (ImGui::InputFloat4(valStr.c_str(), (float*)&val))
                                    {
                                        mat[i] = val;
                                        set = true;
                                    }
                                    ImGui::PopID();
                                }

                                if (set)
                                {
                                    switch (m_workspace->GetCurrentActionType())
                                    {
                                    case ActionType_SetShaderVariableValue:
                                    {
                                        SetShaderVariableValueAction* action = (SetShaderVariableValueAction*)m_workspace->GetCurrentAction();
                                        if (action->GetIndex() == i)
                                        {
                                            action->SetData(&mat);
                                            action->Execute();

                                            break;
                                        }
                                    }
                                    default:
                                    {
                                        m_workspace->PushActionSet(new SetShaderVariableValueAction(objs, objectCount, i, VariableType_Mat4, &mat), "Error setting shader value mat4");

                                        break;
                                    }
                                    }
                                }

                                break;
                            }
                            }
                        }
                    }
                }

                ImGui::Unindent();
            }
            
            delete[] objs;
        }
    }
}