#pragma once

class ShaderVertex;
class ShaderPixel;

class ShaderProgram
{
private:
    unsigned int m_handle;

protected:

public:
    static constexpr char* BaseShaderName = "Base Shader";

    static constexpr char* EditorBase = "SHADER_EDITORSTANDARD";
    static constexpr char* EditorBaseSolid = "SHADER_EDITORSTANDARDSOLID";

    static constexpr char* EditorAnimation = "SHADER_ANIMATORSTANDARD";
    static constexpr char* EditorAnimationSolid = "SHADER_ANIMATORSTANDARDSOLID";
    static constexpr char* EditorAnimationSBody = "SHADER_ANIMATORSBODY";
    static constexpr char* EditorAnimationSBodySolid = "SHADER_ANIMATORSBODYSOLID";

    static constexpr char* EditorWeight = "SHADER_WEIGHTSTANDARD";

    static constexpr char* EditorReferenceImage = "SHADER_REFERENCEIMAGE";

    ShaderProgram(ShaderVertex* a_vertexShader, ShaderPixel* a_pixelShader);
    ~ShaderProgram();

    inline unsigned int GetHandle() const
    {
        return m_handle;
    }

    static ShaderProgram* InitProgram(const char* a_name, const char* a_vertexSource, const char* a_pixelSource);

    static void LoadBaseShaders();
};