#include "Datastore.h"

#include <string.h>

#include "glad/glad.h"
#include "stb_image.h"
#include "Texture.h"

Datastore* Datastore::Instance = nullptr;

Datastore::Datastore()
{

}
Datastore::~Datastore()
{
    for (auto iter = m_shaders.begin(); iter != m_shaders.end(); ++iter)
    {
        delete iter->second;
    }

    for (auto iter = m_textures.begin(); iter != m_textures.end(); ++iter)
    {
        delete iter->second;
    }
}

void Datastore::Init()
{
    if (Instance == nullptr)
    {
        Instance = new Datastore();
    }
}
void Datastore::Destroy()
{
    if (Instance != nullptr)
    {
        delete Instance;
        Instance = nullptr;
    }
}

bool Datastore::AddShaderProgram(const char* a_key, ShaderProgram* a_program)
{
    auto iter = Instance->m_shaders.find(a_key);

    if (iter == Instance->m_shaders.end())
    {
        Instance->m_shaders.emplace(a_key, a_program);   

        return true;
    }

    return false;
}
ShaderProgram* Datastore::GetShaderProgram(const char* a_key)
{
    auto iter = Instance->m_shaders.find(a_key);

    if (iter != Instance->m_shaders.end())
    {
        return iter->second;
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