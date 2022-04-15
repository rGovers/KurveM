#pragma once

#include <BulletCollision/CollisionShapes/btCollisionShape.h>

enum e_CollisionShapeType
{
    CollisionShapeType_Null,
    CollisionShapeType_Sphere,
    CollisionShapeType_End
};

class CollisionShape
{
private:

protected:
    btCollisionShape* m_shape;

    CollisionShape() { }
    
public:
    virtual ~CollisionShape() { }

    virtual e_CollisionShapeType GetShapeType()
    {
        return CollisionShapeType_Null;
    }

    inline btCollisionShape* GetShape()
    {
        return m_shape;
    } 
};