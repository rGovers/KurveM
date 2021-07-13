#pragma once

#include <unordered_map>

class ShaderProgram;
class Texture;

class Datastore
{
private:
    static Datastore* Instance;

    std::unordered_map<const char*, ShaderProgram*> m_shaders;
    std::unordered_map<const char*, Texture*>       m_textures;

    Datastore();
protected:

public:
    ~Datastore();

    static void Init();
    static void Destroy();

    static bool AddShaderProgram(const char* a_key, ShaderProgram* a_program);
    static ShaderProgram* GetShaderProgram(const char* a_key);

    static Texture* GetTexture(const char* a_path);
};