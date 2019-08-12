#include "BoundingBox.h"
#include "BoundingSphere.h"
#include "EmptyVolume.h"

BoundingSphere::BoundingSphere() : BoundingVolume(), mRadius(0.0f)
{
}

BoundingSphere::BoundingSphere(const BoundingSphere *sphere) : BoundingSphere()
{
    mPosition = sphere->mPosition;
    mRotationAxis = sphere->mRotationAxis;
    mRotationAngleInDegrees = sphere->mRotationAngleInDegrees;
    mRadius = sphere->mRadius;
}

BoundingSphere* BoundingSphere::Clone() const
{
    return new BoundingSphere(this);
}

BoundingSphere::BoundingSphere(glm::vec3 position, float radius) : BoundingVolume()
{
    mPosition = position;
    mRadius = radius;
}

BoundingSphere::~BoundingSphere()
{
    delete this;
}

void BoundingSphere::SetRadius(float radius)
{
    mRadius = radius;
}

bool BoundingSphere::IsInVolume(BoundingVolume *volume) {
    printf("[S] Check collision at (%f, %f, %f)\n", GetPosition().x, GetPosition().y, GetPosition().z);
    if(dynamic_cast<BoundingSphere*>(volume))
    {
        return BoundingVolume::SphereCollision(this, dynamic_cast<BoundingSphere*>(volume));
    }
    else if (dynamic_cast<BoundingBox*>(volume))
    {
        return BoundingVolume::SphereBoxCollision(this, dynamic_cast<BoundingBox*>(volume));
    }
    else if (dynamic_cast<EmptyVolume*>(volume))
    {
        return BoundingVolume::CollisionWithEmpty(dynamic_cast<EmptyVolume*>(volume), this);
    }
    return false;
}
