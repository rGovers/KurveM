#pragma once

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

#include <list>

class Camera;
class CurveModel;
class Model;
class ShaderProgram;
class Transform;

class Object
{
private:
    static long long ObjectIDNum;

    long long          m_id;
        
    char*              m_name; 
        
    Transform*         m_transform;

    Object*            m_parent;
    std::list<Object*> m_children;

    ShaderProgram*     m_program;

    CurveModel*        m_curveModel;
    Model*             m_displayModel;

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

    glm::mat4 GetGlobalMatrix() const;

    void Draw(Camera* a_camera, const glm::vec2& a_winSize);
};