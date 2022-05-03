#pragma once 

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

#include <BulletCollision/CollisionShapes/btStridingMeshInterface.h>

class LocalModel;
class Object;

class StridingMeshInterface : public btStridingMeshInterface
{
private:
    const LocalModel* m_mesh;

    glm::vec4*        m_verts;

protected:

public:
    StridingMeshInterface(const LocalModel* a_mesh);
    virtual ~StridingMeshInterface();

    inline const LocalModel* GetMesh() const
    {
        return m_mesh;
    }
    void SetMesh(const LocalModel* a_mesh);

    virtual void getLockedVertexIndexBase(unsigned char** a_vertexbase, int& a_numverts, PHY_ScalarType& a_type, int& a_stride, unsigned char** a_indexbase, int& a_indexstride, int& a_numfaces, PHY_ScalarType& a_indicestype, int a_subpart = 0);
	virtual void getLockedReadOnlyVertexIndexBase(const unsigned char** a_vertexbase, int& a_numverts, PHY_ScalarType& a_type, int& a_stride, const unsigned char** a_indexbase, int& a_indexstride, int& a_numfaces, PHY_ScalarType& a_indicestype, int a_subpart = 0) const;

    virtual void unLockVertexBase(int a_subpart) { }
	virtual void unLockReadOnlyVertexBase(int a_subpart) const { }

    virtual int getNumSubParts() const;

	virtual void preallocateVertices(int a_numverts) { }
	virtual void preallocateIndices(int a_numindices) { }

    void Tick(const glm::mat4* a_matrices, unsigned int a_matrixCount);
};