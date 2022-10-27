#include "Datastore.h"

#include <fstream>
#include <string.h>
#include <sstream>

#include "glad/glad.h"
#include "ShaderPixel.h"
#include "ShaderProgram.h"
#include "Shaders/EditorStandardPixel.h"
#include "Shaders/EditorStandardVertex.h"
#include "ShaderVertex.h"
#include "stb_image.h"
#include "Texture.h"

Datastore* Datastore::Instance = nullptr;

Datastore::Datastore()
{
    m_baseVertex = nullptr;
    m_basePixel = nullptr;
}
Datastore::~Datastore()
{
    for (auto iter = m_shaderPrograms.begin(); iter != m_shaderPrograms.end(); ++iter)
    {
        delete iter->second;
    }

    for (auto iter = m_textures.begin(); iter != m_textures.end(); ++iter)
    {
        delete iter->second;
    }

    for (auto iter = m_vertexShaders.begin(); iter != m_vertexShaders.end(); ++iter)
    {
        delete iter->second;
    }
    for (auto iter = m_pixelShaders.begin(); iter != m_pixelShaders.end(); ++iter)
    {
        delete iter->second;
    }

    if (m_baseVertex != nullptr)
    {
        delete m_baseVertex;
        m_baseVertex = nullptr;
    }
    if (m_basePixel != nullptr)
    {
        delete m_basePixel;
        m_basePixel = nullptr;
    }
}

void Datastore::Init()
{
    if (Instance == nullptr)
    {
        Instance = new Datastore();

        printf("Initialized Datastore \n");
    }
}
void Datastore::Destroy()
{
    if (Instance != nullptr)
    {
        delete Instance;
        Instance = nullptr;
        
        printf("Cleared Datastore \n");
    }
}

bool Datastore::AddShaderProgram(const char* a_key, ShaderProgram* a_program)
{
    const auto iter = Instance->m_shaderPrograms.find(a_key);

    if (iter == Instance->m_shaderPrograms.end())
    {
        printf("Added Shader: ");
        printf(a_key);
        printf("\n");

        Instance->m_shaderPrograms.emplace(a_key, a_program);   

        return true;
    }

    return false;
}
ShaderProgram* Datastore::GetShaderProgram(const char* a_key)
{
    const auto iter = Instance->m_shaderPrograms.find(a_key);

    if (iter != Instance->m_shaderPrograms.end())
    {
        return iter->second;
    }

    return nullptr;
}

ShaderProgram* Datastore::LoadShaderProgram(const char* a_vertexPath, const char* a_pixelPath)
{
    const char* vPath = a_vertexPath;
    if (vPath == nullptr)
    {
        vPath = ShaderProgram::BaseShaderName;
    }
    const char* pPath = a_pixelPath;
    if (pPath == nullptr)
    {
        pPath = ShaderProgram::BaseShaderName;
    }

    const std::string str = std::string("[") + vPath + "] [" + pPath + "]";

    ShaderProgram* program = GetShaderProgram(str.c_str());

    if (program == nullptr)
    {
        ShaderVertex* vertexShader = GetVertexShader(a_vertexPath);
        ShaderPixel* pixelShader = GetPixelShader(a_pixelPath);

        program = new ShaderProgram(vertexShader, pixelShader);

        AddShaderProgram(str.c_str(), program);
    }

    return program;
}

std::list<std::string> Datastore::GetVertexPathList() 
{
    std::list<std::string> list;

    for (auto iter = Instance->m_vertexShaders.begin(); iter != Instance->m_vertexShaders.end(); ++iter)
    {
        list.emplace_back(iter->first);
    }

    return list;
}
ShaderVertex* Datastore::GetVertexShader(const char* a_path)
{
    if (a_path == nullptr)
    {
        if (Instance->m_baseVertex == nullptr)
        {
            Instance->m_baseVertex = new ShaderVertex(EDITORSTANDARDVERTEX);
        }

        return Instance->m_baseVertex;
    }

    const auto iter = Instance->m_vertexShaders.find(a_path);
    
    if (iter != Instance->m_vertexShaders.end())
    {
        return iter->second;
    }

    const char* c = strchr(a_path, '.');
    if (c != nullptr)
    {
        if (strcmp(c, ".glsl") == 0 || strcmp(c, ".vert") == 0)
        {
            std::ifstream file = std::ifstream(a_path);
            
            if (file.good())
            {
                printf("Loaded Vertex Shader: ");
                printf(a_path);
                printf("\n");

                std::stringstream stream;

                stream << file.rdbuf();
                
                file.close();

                const std::string str = stream.str();
                
                ShaderVertex* shader = new ShaderVertex(str.c_str());

                Instance->m_vertexShaders.emplace(a_path, shader);

                return shader;
            }
        }
    }

    return nullptr;
}

std::list<std::string> Datastore::GetPixelPathList()
{
    std::list<std::string> list;

    for (auto iter = Instance->m_pixelShaders.begin(); iter != Instance->m_pixelShaders.end(); ++iter)
    {
        list.emplace_back(iter->first);
    }

    return list;
}
ShaderPixel* Datastore::GetPixelShader(const char* a_path)
{
    if (a_path == nullptr)
    {
        if (Instance->m_basePixel == nullptr)
        {
            Instance->m_basePixel = new ShaderPixel(EDITORSTANDARDPIXEL);
        }

        return Instance->m_basePixel;
    }

    const auto iter = Instance->m_pixelShaders.find(a_path);

    if (iter != Instance->m_pixelShaders.end())
    {
        return iter->second;
    }

    const char* c = strchr(a_path, '.');
    if (c != nullptr)
    {
        if (strcmp(c, ".glsl") == 0 || strcmp(c, ".frag") == 0 || strcmp(c, ".pix") == 0)
        {
            std::ifstream file = std::ifstream(a_path);

            if (file.good())
            {
                printf("Loaded Pixel Shader: ");
                printf(a_path);
                printf("\n");

                std::stringstream stream;

                stream << file.rdbuf();

                file.close();

                const std::string str = stream.str();

                ShaderPixel* shader = new ShaderPixel(str.c_str());

                Instance->m_pixelShaders.emplace(a_path, shader);

                return shader;
            }
        }
    }

    return nullptr;
}

Texture* Datastore::GetTexture(const char* a_path)
{
    auto iter = Instance->m_textures.find(a_path);

    if (iter != Instance->m_textures.end())
    {
        return iter->second;
    }

    const char* c = strchr(a_path, '.');

    if (c != nullptr)
    {
        if (strcmp(c, ".png") == 0 || strcmp(c, ".bmp") == 0 || strcmp(c, ".jpg") == 0)
        {
            printf("Loaded Texture: ");
            printf(a_path);
            printf("\n");

            int width;
            int height;

            int comp;

            unsigned char* data = stbi_load(a_path, &width, &height, &comp, STBI_rgb_alpha);
            if (data != nullptr)
            {
                Texture* tex = new Texture(width, height, GL_RGBA, GL_RGBA, data);

                Instance->m_textures.emplace(a_path, tex);

                stbi_image_free(data);

                return tex;
            }
        }
    }

    return nullptr;
}