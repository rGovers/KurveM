#pragma once

#include <list>
#include <string>
#include <unordered_map>

class ShaderPixel;
class ShaderProgram;
class ShaderVertex;
class Texture;

class Datastore
{
private:
    static Datastore* Instance;

    ShaderVertex*                                   m_baseVertex;
    ShaderPixel*                                    m_basePixel;

    std::unordered_map<std::string, ShaderProgram*> m_shaderPrograms;

    std::unordered_map<std::string, ShaderVertex*>  m_vertexShaders;
    std::unordered_map<std::string, ShaderPixel*>   m_pixelShaders;

    std::unordered_map<std::string, Texture*>       m_textures;

    Datastore();
    
protected:

public:
    ~Datastore();

    static void Init();
    static void Destroy();

    static bool AddShaderProgram(const char* a_key, ShaderProgram* a_program);
    static ShaderProgram* GetShaderProgram(const char* a_key);

    static ShaderProgram* LoadShaderProgram(const char* a_vertexPath, const char* a_pixelPath);

    static std::list<std::string> GetVertexPathList();
    static ShaderVertex* GetVertexShader(const char* a_path);

    static std::list<std::string> GetPixelPathList();
    static ShaderPixel* GetPixelShader(const char* a_path);

    static Texture* GetTexture(const char* a_path);
};