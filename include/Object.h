#pragma once

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

#include <fstream>
#include <list>

#include "tinyxml2.h"

class Camera;
class CurveModel;
class Model;
class ShaderProgram;
class Texture;
class Transform;

class Object
{
private:
    static long long ObjectIDNum;

    long long          m_id;
        
    char*              m_name; 
        
    char*              m_referencePath;
    Texture*           m_referenceImage;

    Transform*         m_transform;

    Object*            m_parent;
    std::list<Object*> m_children;

    ShaderProgram*     m_program;
    ShaderProgram*     m_referenceProgram;

    CurveModel*        m_curveModel;

protected:

public:
    Object(const char* a_name);
    ~Object();

    inline const char* GetName() const
    {
        return m_name;
    }
    void SetName(const char* a_name);

    inline long long GetID() const
    {
        return m_id;
    }

    inline Transform* GetTransform() const
    {
        return m_transform;
    }

    inline Object* GetParent() const
    {
        return m_parent;
    }
    void SetParent(Object* a_parent);

    inline std::list<Object*> GetChildren() const
    {
        return m_children;
    }

    inline CurveModel* GetCurveModel() const
    {
        return m_curveModel;
    }
    inline void SetCurveModel(CurveModel* a_curveModel)
    {
        m_curveModel = a_curveModel;
    }

    bool SetReferenceImage(const char* a_path);

    glm::mat4 GetGlobalMatrix() const;

    void Draw(Camera* a_camera, const glm::vec2& a_winSize);

    void WriteOBJ(std::ofstream* a_file, bool a_smartStep, int a_steps) const;
    void Serialize(tinyxml2::XMLDocument* a_doc, tinyxml2::XMLElement* a_element) const;

    static Object* ParseData(const tinyxml2::XMLElement* a_element, Object* a_parent);
};