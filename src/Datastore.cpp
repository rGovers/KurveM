#include "Datastore.h"

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