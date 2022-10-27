#pragma once

#include "Modals/Modal.h"

#include <list>

class Object;
class Workspace;

enum e_ShaderLoadMode
{
    ShaderLoadMode_Vertex,
    ShaderLoadMode_Pixel
};

class LoadShaderModal : public Modal
{
private:
    static constexpr int PathSize = 2048;

    static constexpr int VertexExtCount = 2;
    static constexpr char* VertexFileExts[VertexExtCount] = { ".glsl", ".vert" };

    static constexpr int PixelExtCount = 3;
    static constexpr char* PixelFileExts[PixelExtCount] = { ".glsl", ".frag", ".pix" };

    Workspace*       m_workspace;

    e_ShaderLoadMode m_mode;

    unsigned int     m_objectCount;
    Object**         m_objects;

    char*            m_path;
    char*            m_name;

    char*            m_fPath;

    std::list<char*> m_dirs;
    std::list<char*> m_files;

    void Clear();
    void GenerateDirectoryData();

protected:

public:
    LoadShaderModal(Workspace* a_workspace, const char* a_path, Object* const* a_objects, unsigned int a_objectCount, e_ShaderLoadMode a_mode);
    ~LoadShaderModal();

    virtual const char* GetName();

    virtual glm::vec2 GetSize();

    virtual bool Execute();
};